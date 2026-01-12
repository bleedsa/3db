#include <sys/socket.h>

#include <str.h>
#include <net/Q.h>

static constexpr S headZ = 1+Z(S);

auto Net::send_Q(int sock, Q::Q *x) -> char* {
	int sent;
	u8 head[headZ], *body;
	S L, z;

	/* the first byte is the type */
	head[0] = x->ty;
	/* memcpy the length */
	L = x->len();
	memcpy(head+1, &L, Z(S));

	/* send the header */
	dbg(std::cout << "sending Q header of " << Fmt::Fmt(x) << "...");
	if ((sent = send(sock, (const void*)head, headZ, 0)) == -1) {
		return A_err("failed to send Q header: {}", strerror(errno));
	}
	dbg(std::cout << "ok " << sent << std::endl);

	/* make the body and send it */
	z = x->atom_size() * L;
	body = mk<u8>(z);
	x->fill_buf_with_elems(body);

	/* send the body */
	dbg(std::cout << "sending Q body...");
	if ((sent = send(sock, (const void*)body, z, 0)) == -1) {
		return A_err("failed to send Q body: {}", strerror(errno));
	}
	dbg(std::cout << "ok " << sent << std::endl);

	return nullptr;
}

auto Net::recv_Q(int sock, Q::Q *x) -> char* {
	int got;
	u8 head[headZ], *body;
	S L = -1, z;

	/* recv the header */
	dbg(std::cout << "recv'ing Q header...");
	if ((got = recv(sock, (void*)head, headZ, 0)) == -1) {
		return A_err("failed to recv Q header: {}", strerror(errno));
	}
	dbg(std::cout << "ok " << got << std::endl);

	/* decode */
	x->ty = (Q::QTy)head[0];
	memcpy(&L, head+1, Z(S));

	std::cout << "header of " << x->short_name();
	std::cout << " with len " << L << std::endl;

	/* make a buffer to hold the body */
	z = x->atom_size() * L;
	body = mk<u8>(z);

	/* recv body */
	dbg(std::cout << "recv'ing Q body...");
	if ((got = recv(sock, (void*)body, z, 0)) == -1) {
		free(body);
		return A_err("failed to recv Q body: {}", strerror(errno));
	}
	dbg(std::cout << "ok " << got << std::endl);

	/* decode */
	*x = Q::Q(x->ty, body, L);

	free(body);
	return nullptr;
}
