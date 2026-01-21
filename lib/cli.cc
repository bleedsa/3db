#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>

#include <cli.h>
#include <net.h>
#include <net/Q.h>
#include <net/Asm.h>

inl auto connect_host(const char *addr_port) -> int {
	int ret, sock, on;
	const char *addr, *port;
	struct addrinfo hints, *res;
	auto r = Net::parse_addr_port((char*)addr_port);
	if (!r) fatal("failed to parse address: {}", r.error());
	auto [addr_str, port_str] = *r;

	addr = addr_str.c_str();
	port = port_str.c_str();

	/* get address info */
	memset(&hints, 0, Z(hints));
	hints.ai_family = AF_UNSPEC; /* v4 or v6 */
	hints.ai_socktype = SOCK_STREAM;
	if ((ret = getaddrinfo(addr, port, &hints, &res)) != 0) {
		fatal("getaddrinfo(): {}", gai_strerror(ret));
	}

	/* make socket */
	sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if (sock == -1) fatal("socket(): {}", strerror(errno));

	/* make connection */
	ret = connect(sock, res->ai_addr, res->ai_addrlen);
	if (ret < 0) fatal("connect(): {}", strerror(errno));

	return sock;
}

Cli::Cli::Cli(const char *a) {
	auto L = strlen(a) + 1;
	addr = mk<char>(L);
	memmove(addr, a, Z(char)*L);
	addr[L-1]=0;
}

Cli::Cli::~Cli() {
	free(addr);
}

inl auto send_asm(Cli::Cli *cli, Asm::Asm *a) -> R<Q::Q> {
	Q::Q q;
	char *err;
	auto sock = connect_host(cli->addr);
	R<Q::Q> r;

	/* send the asm, get a response */
	if ((err = Net::send_Asm(sock, a))) r = std::unexpected(err);
	if ((err = Net::recv_Q(sock, &q))) r = std::unexpected(err);
	else r = q;

	close(sock);
	return r;
}

template<typename X>
inl auto set_atom(Cli::Cli *cli, var_t n, X x) -> R<Q::Q> {
	auto a = Asm::Asm();
	/* push the default body */
	a.push_bod(VM::Bod(0, 0));

	a.push_in(x); a.push_in(Bc::In(Bc::STORE, n));
	a.push_in(Bc::In(Bc::LOAD, n));

	return send_asm(cli, &a);
}

template<typename X>
inl auto set_vec(Cli::Cli *cli, var_t n, A::A<X> *v, Bc::InTy Y) -> R<Q::Q> {
	auto a = Asm::Asm();
	/* push the default body */
	a.push_bod(VM::Bod(0, 0));

	v->for_each([&](X *x){a.push_in(*x);}); /* push each elem */
	a.push_in((i32)v->len);                 /* push the len */
	a.push_in(Y);                           /* the vec constructor instr */
	a.push_in(Bc::In(Bc::STORE, n));        /* the store instr */
	a.push_in(Bc::In(Bc::LOAD, n));

	return send_asm(cli, &a);
}
	
auto Cli::Cli::set(var_t n, Q::Q *q) -> R<Q::Q> {
	switch (q->ty) {
	/* atoms are a simple LIT + STORE */
	CASE(Q::QInt, return set_atom<i32>(this, n, q->i))
	CASE(Q::QDbl, return set_atom<f64>(this, n, q->d))
	CASE(Q::QChr, return set_atom<Chr>(this, n, q->c))
	/* vecs are more complicated */
	CASE(Q::QINT, return set_vec<i32>(this, n, &q->iA, Bc::MKAi32))
	CASE(Q::QDBL, return set_vec<f64>(this, n, &q->dA, Bc::MKAf64))
	CASE(Q::QCHR, return set_vec<Chr>(this, n, &q->cA, Bc::MKAChr))
	default: return err_fmt("cannot set Q of type {}", q->short_name());
	}
}
