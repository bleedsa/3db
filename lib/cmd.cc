#include <cmd.h>
#include <net.h>
#include <net/Cmd.h>

#define N(x) [x] = #x
const char *Cmd::CmdTy_names[] = {
	N(NIL), N(SELECT), N(INSERT), N(CREATE), N(GET),
};

Cmd::Cmd::Cmd(int sock) : sock{sock} {
	char *err;
	if ((err = Net::recv_Cmd(sock, this))) throw err;
}

Cmd::Cmd::Cmd(CmdTy ty, int sock) : ty{ty}, sock{sock} {
	switch (ty) {
	CASE(CREATE, create=Create())
	CASE(INSERT, insert=Insert())
	CASE(GET,    get=Get())
	}
}

Cmd::Cmd::Cmd(CmdTy ty, const char *addr)
	: ty{ty}, sock{Net::connect_host(addr)}
{
	switch (ty) {
	CASE(CREATE, create=Create())
	CASE(INSERT, insert=Insert())
	CASE(GET,    get=Get())
	}
}

Cmd::Cmd::~Cmd() {
	switch (ty) {
	CASE(CREATE, create.~Create())
	CASE(INSERT, insert.~Insert())
	CASE(GET,    get.~Get())
	}
}

inl auto Cmd::Cmd::cpy(const Cmd &x) -> void {
	ty = x.ty, sock = x.sock;
	switch (ty) {
	CASE(CREATE, create = x.create)
	CASE(INSERT, insert = x.insert)
	CASE(GET,    get = x.get)
	}
}

Cmd::Cmd::Cmd(const Cmd &x) {cpy(x);}
auto Cmd::Cmd::operator=(const Cmd &x)->Cmd&{cpy(x);return *this;}
