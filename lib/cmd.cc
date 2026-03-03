#include <cmd.h>
#include <net.h>
#include <net/Cmd.h>

#define N(x) [x] = #x
const char *Cmd::CmdTy_names[] = {N(NIL), N(SELECT), N(INSERT), N(CREATE)};

Cmd::Cmd::Cmd(int sock) : sock{sock} {
	char *err;
	if ((err = Net::recv_Cmd(sock, this))) throw err;
}

Cmd::Cmd::Cmd(CmdTy ty, int sock) : ty{ty}, sock{sock} {
	switch (ty) {
	CASE(CREATE, create=Create())
	CASE(INSERT, insert=Insert())
	}
}

Cmd::Cmd::Cmd(CmdTy ty, const char *addr)
	: ty{ty}, sock{Net::connect_host(addr)}
{
	switch (ty) {
	CASE(CREATE, create=Create())
	CASE(INSERT, insert=Insert())
	}
}

Cmd::Cmd::~Cmd() {
	switch (ty) {
	CASE(CREATE, create.~Create())
	CASE(INSERT, insert.~Insert())
	}
}

inl auto Cmd::Cmd::cpy(const Cmd &x) -> void {
	ty = x.ty, sock = x.sock;
	switch (ty) {
	CASE(CREATE, create = x.create)
	CASE(INSERT, insert = x.insert)
	}
}

Cmd::Cmd::Cmd(const Cmd &x) {cpy(x);}
auto Cmd::Cmd::operator=(const Cmd &x)->Cmd&{cpy(x);return *this;}

/*
auto Cmd::Insert::insert(T::T *t) -> R<void> {
}

auto Cmd::Insert::exe() -> R<Db::Ent*> {
	auto o = Db::get(name);
	if (!o) {
		auto n = var_to_str(name);
		auto r = err_fmt("table {} not found.", n);
		delete[] n;
		return r;
	}

	auto e = *o;
	if (e->ty != Db::Tab) {
		auto n = var_to_str(name);
		auto r = err_fmt("entry {} is not a table.", n);
		delete[] n;
		return r;
	}

	auto t = e->as_T();
	t->reZ(row);

	insert(t);

	return e;
}
*/
