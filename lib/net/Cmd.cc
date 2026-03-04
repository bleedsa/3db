#include <sys/socket.h>

#include <cmd.h>
#include <net.h>
#include <net/Cmd.h>
#include <net/Q.h>
#include <net/Ent.h>

auto Cmd::Cmd::send() -> R<Db::Ent> {
	char *err;
	Db::Ent ent(empty_var(), -1);

	if ((err = Net::send_Cmd(sock, this))) {
		return err_fmt("failed to send command: {}", err);
	}

	if ((err = Net::recv_Ent(sock, &ent))) {
		return err_fmt("failed to recv entry: {}", err);
	}

	return R<Db::Ent>(ent);
}

auto Net::send_Cmd(int sock, Cmd::Cmd *x) -> char* {
	auto tcp = Net::TcpS(sock);

	u8 t = x->ty;
	tcp << t;

	switch (x->ty) {
	[[unlikely]]
	CASE(Cmd::NIL, return "send() called on NIL")
	CASE(Cmd::INSERT, return send_Insert(sock, &x->insert))
	CASE(Cmd::CREATE, return send_Create(sock, &x->create))
	CASE(Cmd::GET, return send_Get(sock, &x->get))
	CASE(Cmd::SET, return send_Set(sock, &x->set))
	}

	return nullptr;
}

auto Net::recv_Cmd(int sock, Cmd::Cmd *x) -> char* {
	auto tcp = Net::TcpS(sock);

	u8 t;
	tcp >> t;
	x->ty = (Cmd::CmdTy)t, x->sock = sock;

	switch (x->ty) {
	[[unlikely]]
	CASE(Cmd::NIL, return "recv() called on NIL")
	CASE(Cmd::INSERT, return recv_Insert(sock, &x->insert))
	CASE(Cmd::CREATE, return recv_Create(sock, &x->create))
	CASE(Cmd::GET, return recv_Get(sock, &x->get))
	CASE(Cmd::SET, return recv_Set(sock, &x->set))
	}

	return nullptr;
}


constexpr S Insert_headZ = Z(var_t) + (2*Z(u64));

template<typename X>
inl auto set_header_field(u8 *ptr, X x) -> S {
	memcpy(ptr, &x, Z(X));
	return Z(X);
}

auto Net::send_Insert(int sock, Cmd::Insert *x) -> char* {
	u64 i /* idx */, L = (u64)x->cols.len /* length of the cols vec */;
	u8 head[Insert_headZ], *body, *ptr; /* buffers */
	int sent;
	char *err;
	Q::Q *q;

	/* setup the header */
	ptr = head;
	ptr += set_header_field(ptr, x->name);
	ptr += set_header_field(ptr, (u64)x->row);
	ptr += set_header_field(ptr, L);

	/* send */
	dbg(std::cout << "sending Insert header...");
	if ((sent = send(sock, (const void*)head, Insert_headZ, 0)) == -1) {
		return A_err("failed to send Cmd header: {}", strerror(errno));
	}
	dbg(std::cout << "ok " << sent << std::endl);

	/* send each Q in the cols */
	if (!x->cols.ptr) [[unlikely]] goto end;
	else {
		for (i = 0; i < L; i++) {
			/* find q */
			q = x->cols.ptr+i;
			/* send */
			if ((err = send_Q(sock, q))) {
				return A_err(
					"failed to send Cmd column {}: {}",
					Fmt::Fmt(q), err
				);
			}
		}
	}

end:
	return nullptr;
}

template<typename X>
inl auto get_header_field(u8 *ptr, X *x) -> S {
	memcpy(x, ptr, Z(X));
	return Z(X);
}

auto Net::recv_Insert(int sock, Cmd::Insert *x) -> char* {
	int got;
	u8 head[Insert_headZ], *ptr;
	u64 i, L, row = 0;
	Q::Q *q;
	char *err;

	/* do the first recv */
	dbg(std::cout << "recv'ing Cmd header...");
	if ((got = recv(sock, (void*)head, Insert_headZ, 0)) == -1) {
		return A_err("failed to recv Cmd header: {}", strerror(errno));
	} else if (got == 0) {
		return A_err("connection reset: {}", strerror(errno));
	}
	dbg(std::cout << "ok " << got << std::endl);

	/* decode the header */
	ptr = head;
	ptr += get_header_field<var_t>(ptr, &x->name);
	ptr += get_header_field<u64>(ptr, &row); x->row = (S)row;
	ptr += get_header_field<u64>(ptr, &L);
	
	/* setup Q x */
	x->cols = A::A<Q::Q>(L);

	/* recv each Q in the columns */
	for (i = 0; i < L; i++) {
		q = x->cols.ptr+i;
		if ((err = recv_Q(sock, q))) {
			return A_err(
				"failed to recv Cmd column {}: {}",
				i, err
			);
		}
	}

	return nullptr;
}

constexpr S Create_headZ = Z(var_t) + Z(Db::EntTy);

auto Net::send_Create(int sock, Cmd::Create *x) -> char* {
	auto tcp = TcpS(sock);

	u8 e = (u8)x->ty;
	auto has_cols = x->cols.has_value() && x->cols->len != 0;

	try {
		tcp << e << x->name << has_cols;

		if (has_cols) {
			auto cols = *x->cols;
			/* make the name and type column buffers */
			auto L = cols.len;
			auto names = new var_t[L];
			auto types = new T::TColTy[L];
			for (S i = 0; i < L; i++) {
				auto &[n, t] = cols[i];
				names[i] = n, types[i] = t;
			}

			/* send everything */
			tcp
				<< std::tuple{(u8*)names, Z(var_t)*L}
				<< std::tuple{(u8*)types, Z(T::TColTy)*L};
		}
	} catch (std::string &e) {
		return A_err("failed to send Create: {}", e);
	}

	return nullptr;
}

auto Net::recv_Create(int sock, Cmd::Create *x) -> char* {
	auto tcp = TcpS(sock);
	u8 e;
	bool has_cols;

	/* recv */
	try {
		tcp >> e >> x->name >> has_cols;

		if (has_cols) {
			auto tup = std::tuple<u8*, u64>{nullptr, 0};
			tcp >> tup;
			auto names = (var_t*)std::get<0>(tup);
			tcp >> tup;
			auto types = (T::TColTy*)std::get<0>(tup);
			auto L = std::get<1>(tup);

			x->cols = A::A<Cmd::Col>(L);

			for (S i = 0; i < L; i++) {
				x->cols->at(i) = std::tuple{names[i], types[i]};
			}
		} else {
			x->cols = {};
		}
	} catch (std::string &e) {
		return A_err("failed to recv Create: {}", e);
	}

	/* decode */
	x->ty = (Db::EntTy)e;

	return nullptr;
}

auto Net::send_Get(int sock, Cmd::Get *x) -> char* {
	auto tcp = TcpS(sock);

	try {
		tcp << x->name;
	} catch (std::string &e) {
		return A_err("failed to send Get: {}", e);
	}

	return nullptr;
}

auto Net::recv_Get(int sock, Cmd::Get *x) -> char* {
	auto tcp = TcpS(sock);

	try {
		tcp >> x->name;
	} catch (std::string &e) {
		return A_err("failed to recv Get: {}", e);
	}

	return nullptr;
}

auto Net::send_Set(int sock, Cmd::Set *x) -> char* {
	auto tcp = TcpS(sock);

	try {
		tcp << x->name << x->val;
	} catch (std::string &e) {
		return A_err("failed to send Set: {}", e);
	}

	return nullptr;
}

auto Net::recv_Set(int sock, Cmd::Set *x) -> char* {
	auto tcp = TcpS(sock);

	try {
		tcp >> x->name >> x->val;
	} catch (std::string &e) {
		return A_err("failed to recv Set: {}", e);
	}

	return nullptr;
}
