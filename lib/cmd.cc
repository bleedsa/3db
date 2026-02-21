#include <cmd.h>
#include <net.h>
#include <net/Cmd.h>

#define N(x) [x] = #x
const char *Cmd::CmdTy_names[] = {N(NIL), N(SELECT), N(INSERT), N(CREATE)};

Cmd::Cmd::Cmd(int fd) : net{true}, sock{fd} {
	*this = Net::get_cmd(sock);
}


Cmd::Cmd::Cmd(CmdTy ty, int fd)
	: ty{ty}, net{true}, sock{fd}
{}

Cmd::Cmd::Cmd(CmdTy ty, const char *addr)
	: ty{ty}, net{true}, sock{Net::connect_host(addr)}
{}

Cmd::Cmd::Cmd(CmdTy ty) : ty{ty}, net{false}, sock{-1} {}

Cmd::Cmd::Cmd(const Cmd &x) {cpy(x);}

auto Cmd::Cmd::operator=(const Cmd &x) -> Cmd& {
	cpy(x);
	return *this;
}

auto Cmd::Cmd::entry(var_t name) -> Cmd& {
	switch (ty) {
	CASE(NIL, throw "entry() called on NIL")
	CASE(INSERT, dynamic_cast<Insert*>(this)->name = name)
	CASE(CREATE, dynamic_cast<Create*>(this)->name = name)
	}
	return *this;
}

auto Cmd::Cmd::entry(const char *name) -> Cmd& {
	auto n = str_to_var(name);
	return entry(n);
}

auto Cmd::Cmd::exe() -> R<Db::Ent*> {
	switch (ty) {
	CASE(NIL, throw "exe() called on NIL")
	CASE(INSERT, return dynamic_cast<Insert*>(this)->exe())
	CASE(CREATE, return dynamic_cast<Create*>(this)->exe())
	}
}

Cmd::Create::Create() : Cmd(CREATE), name{empty_var()} {}

Cmd::Create::Create(int fd)
	: Cmd(CREATE, fd)
{
	auto err = Net::recv_Create(fd, this);
	if (err) throw err;
}

Cmd::Create::Create(const char *addr)
	: Cmd(CREATE, addr), name{empty_var()}
{}

Cmd::Create::~Create() {}

auto Cmd::Create::operator=(const Create &x) -> Create& {
	Cmd::operator=(x);
	cpy(x);
	return *this;
}

Cmd::Create::Create(const Create &x) : Cmd(x) {cpy(x);}
auto Cmd::Create::entry(var_t name) -> Create& {this->name = name;return *this;}
auto Cmd::Create::entry(const char *name) -> Create& {
	auto n = str_to_var(name);
	return entry(n);
}

auto Cmd::Create::exe() -> R<Db::Ent*> {
	switch (entty) {
	CASE(Db::Int, return R<Db::Ent*>(Db::add(name, 0)))
	CASE(Db::Dbl, return R<Db::Ent*>(Db::add(name, 0.0)))
	CASE(Db::Ch,  return R<Db::Ent*>(Db::add(name, '\0')))
	CASE(Db::INT, return R<Db::Ent*>(Db::add(name, A::A<i32>(0))))
	CASE(Db::DBL, return R<Db::Ent*>(Db::add(name, A::A<f64>(0))))
	CASE(Db::CHR, return R<Db::Ent*>(Db::add(name, A::A<Chr>(0))))
	}
}

Cmd::Insert::Insert()
	: Cmd(INSERT), name{empty_var()}, row{0}, cols{A::A<Q::Q>(0)}
{}

Cmd::Insert::Insert(int fd)
	: Cmd(INSERT, fd), cols{A::A<Q::Q>(0)}
{
	auto err = Net::recv_Insert(fd, this);
	if (err) throw err;
}

Cmd::Insert::Insert(const char *addr)
	: Cmd(INSERT, addr), name{empty_var()}, row{0}, cols{A::A<Q::Q>(0)}
{}

Cmd::Insert::~Insert() {}

auto Cmd::Insert::operator=(const Insert &x) -> Insert& {
	Cmd::operator=(x);
	cpy(x);
	return *this;
}

Cmd::Insert::Insert(const Insert &x) : Cmd(x), cols{x.cols} {cpy(x);}
auto Cmd::Insert::entry(var_t name) -> Insert& {this->name = name;return *this;}
auto Cmd::Insert::entry(const char *name) -> Insert& {
	auto n = str_to_var(name);
	return entry(n);
}

template<typename X>
inl auto set_col(T::T *t, S row, S col, X x) -> void {
	((X*)t->cols[col])[row] = x;
}

auto Cmd::Insert::insert(T::T *t) -> R<void> {
	S col;
	Q::Q *q;

	/* a little safety */
	[[unlikely]]
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
		[[unlikely]]
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
