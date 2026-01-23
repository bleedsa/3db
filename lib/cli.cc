
#include <cli.h>
#include <net.h>
#include <net/Q.h>
#include <net/Asm.h>

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
	auto sock = Net::connect_host(cli->addr);
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

	return send_asm(cli, &a);
}

template<typename X>
inl auto set_vec(Cli::Cli *cli, var_t n, A::A<X> *v, Bc::InTy Y) -> R<Q::Q> {
	auto a = Asm::Asm();
	/* push the default body */
	a.push_bod(VM::Bod(0, 0));

	/* push each elem */
	auto r = v->rev();
	r.for_each([&](X *x){a.push_in(*x);}); 
	/* push the len */
	a.push_in((i32)v->len);
	/* the vec constructor instr */
	a.push_in(Y);
	/* the store instr */
	a.push_in(Bc::In(Bc::STORE, n));

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

auto Cli::Cli::get(var_t n) -> R<Q::Q> {
	auto a = Asm::Asm();
	a.push_bod(VM::Bod(0, 0));
	a.push_in(Bc::In(Bc::LOAD, n));
	return send_asm(this, &a);
}
