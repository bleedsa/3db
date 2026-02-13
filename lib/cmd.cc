#include <cmd.h>

#define N(x) [x] = #x
const char *Cmd::CmdTy_names[] = {N(NIL), N(SELECT), N(INSERT)};

Cmd::Cmd::Cmd(const char *addr)
	: ty{NIL}, net{true}, sock{Net::connect_host(addr)}
{}

Cmd::Cmd::Cmd(CmdTy ty) : ty{ty}, net{false}, sock{-1} {}

Cmd::Cmd::~Cmd() {
	if (net) close(sock);
}

Cmd::Cmd::Cmd(const Cmd &x) : ty{x.ty}, net{x.net}, sock{x.sock} {}

auto Cmd::Cmd::operator=(const Cmd &x) -> Cmd& {
	ty = x.ty, net = x.net, sock = x.sock;
	return *this;
}

template<typename X>
inl auto set_col(T::T *t, S row, S col, X x) -> void {
	((X*)t->cols[col])[row] = x;
}

auto Cmd::Insert::insert(T::T *t) -> R<void> {
	S col;
	Q::Q *q;

	/* a little safety */
	if (t->coln != cols.len) {
		auto n = var_to_str(name);
		auto r = err_fmt(
			"attempting to insert {} columns into table with"
			"{} columns.",
			cols.len, t->coln
		);
		delete[] n;
		return r;
	}

	/* iterate col by col */
	for (col = 0, q = &cols[col]; col < cols.len; col++, q = &cols[col]) {
		switch (q->ty) {
		/* safety with nil */
		CASE(Q::QNil, {
			auto n = var_to_str(name);
			auto r = err_fmt("attempting to insert nil into {}", n);
			delete[] n;
			return r;
		})

		/* set col calls */
		CASE(Q::QInt, set_col<i32>(t, row, col, q->i))
		CASE(Q::QDbl, set_col<f64>(t, row, col, q->d))
		CASE(Q::QChr, set_col<Chr>(t, row, col, q->c))
		CASE(Q::QINT, set_col<A::A<i32>>(t, row, col, q->iA))
		CASE(Q::QDBL, set_col<A::A<f64>>(t, row, col, q->dA))
		CASE(Q::QCHR, set_col<A::A<Chr>>(t, row, col, q->cA))

		/* die */
		default: std::unreachable();
		}
	};

	return {};
}

auto Cmd::Insert::exe() -> R<Db::Ent*> {
	/* find */
	auto o = Db::get(name);
	if (!o) {
		auto n = var_to_str(name);
		auto r = err_fmt("table {} not found.", n);
		delete[] n;
		return r;
	}

	/* check if we got back a table */
	auto e = *o;
	if (e->ty != Db::Tab) {
		auto n = var_to_str(name);
		auto r = err_fmt("entry {} is not a table.", n);
		delete[] n;
		return r;
	}

	/* grab the table and make room for the row */
	auto t = e->as_T();
	t->reZ(row);

	/* perform the insertion */
	insert(t);

	return e;
}
