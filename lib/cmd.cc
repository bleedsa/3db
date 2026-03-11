#include <cmd.h>
#include <net.h>
#include <net/Cmd.h>

#define N(x) [x] = #x
const char *Cmd::CmdTy_names[] = {
	N(NIL), N(SELECT), N(INSERT), N(CREATE), N(GET), N(SET),
};

Cmd::Cmd::Cmd(int sock) : sock{sock} {
	char *err;
	if ((err = Net::recv_Cmd(sock, this))) throw err;
}

Cmd::Cmd::Cmd(CmdTy ty, int sock) : ty{ty}, sock{sock} {
	switch (ty) {
	CASE(SELECT, select=Select())
	CASE(CREATE, create=Create())
	CASE(INSERT, insert=Insert())
	CASE(GET,    get=Get())
	CASE(SET,    set=Set())
	}
}

Cmd::Cmd::Cmd(CmdTy ty, const char *addr)
	: ty{ty}, sock{Net::connect_host(addr)}
{
	switch (ty) {
	CASE(SELECT, select=Select())
	CASE(CREATE, create=Create())
	CASE(INSERT, insert=Insert())
	CASE(GET,    get=Get())
	CASE(SET,    set=Set())
	}
}

Cmd::Cmd::~Cmd() {
	switch (ty) {
	CASE(SELECT, select.~Select())
	CASE(CREATE, create.~Create())
	CASE(INSERT, insert.~Insert())
	CASE(GET,    get.~Get())
	CASE(SET,    set.~Set())
	}
}

inl auto Cmd::Cmd::cpy(const Cmd &x) -> void {
	ty = x.ty, sock = x.sock;
	switch (ty) {
	CASE(SELECT, select = x.select)
	CASE(CREATE, create = x.create)
	CASE(INSERT, insert = x.insert)
	CASE(GET,    get = x.get)
	CASE(SET,    set = x.set)
	}
}

Cmd::Cmd::Cmd(const Cmd &x) {cpy(x);}
auto Cmd::Cmd::operator=(const Cmd &x)->Cmd&{cpy(x);return *this;}

auto Cmd::Cmd::entry(var_t name) -> Cmd& {
	switch (ty) {
	CASE(SELECT, select.name = name)
	CASE(CREATE, create.name = name)
	CASE(INSERT, insert.name = name)
	CASE(GET,    get.name = name)
	CASE(SET,    set.name = name)
	default: throw str_fmt(
		"{} has no entry field", type_name()
	);
	}
	return *this;
}

auto Cmd::Cmd::entry(const char *name) -> Cmd& {
	auto n = str_to_var(name);
	return entry(n);
}

auto Cmd::Cmd::type(Db::EntTy ty) -> Cmd& {
	switch (this->ty) {
	CASE(CREATE, create.ty = ty)
	default: throw str_fmt(
		"{} has no type field", type_name()
	);
	}
	return *this;
}

auto Cmd::Cmd::row(S row) -> Cmd& {
	switch (ty) {
	CASE(INSERT, insert.row = row)
	default: throw str_fmt(
		"{} has no row field", type_name()
	);
	}
	return *this;
}

auto Cmd::Cmd::columns(std::vector<Q::Q> cols) -> Cmd& {
	switch (ty) {
	CASE(INSERT, insert.cols = cols)
	default: throw str_fmt(
		"{} has no columns field", type_name()
	);
	}
	return *this;
}

auto Cmd::Cmd::columns(
	std::vector<std::tuple<const char*, T::TColTy>> cols
) -> Cmd& {
	switch (ty) {
	case CREATE: {
		auto r = std::vector<Col>();
		for (auto &[name, type] : cols) {
			auto n = str_to_var(name);
			r.push_back(std::tuple{n, type});
		}
		create.cols = r;
		break;
	}
	default: throw str_fmt(
		"{} has no valid columns field", type_name()
	);
	}
	return *this;
}

auto Cmd::Cmd::columns(A::A<var_t> cols) -> Cmd& {
	switch (ty) {
	CASE(SELECT, select.cols = cols)
	default: throw str_fmt(
		"{} has no valid columns field", type_name()
	);
	}
	return *this;
}

auto Cmd::Cmd::columns(A::A<const char*> cols) -> Cmd& {
	auto x = cols.each<var_t>([](const char **x){return str_to_var(*x);});
	return columns(x);
}

auto Cmd::Cmd::value(Q::Q val) -> Cmd& {
	switch (ty) {
	CASE(SET, set.val = val)
	default: throw str_fmt(
		"{} has no valid value field", type_name()
	);
	}
	return *this;
}
