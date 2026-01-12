#include <Asm.h>
#include <db.h>

inl auto stk_pop(std::vector<Q::Q> *s) -> Q::Q {
	i64 i = s->size() - 1;
	auto r = i >= 0 ? s->at(i) : Q::Q();
	s->pop_back();
	return r;
}

inl auto store_Q(std::vector<Q::Q> *s, Bc::In *in) -> void {
	auto x = stk_pop(s);

	switch (x.ty) {
	CASE(Q::QInt, Db::push_ent(in->var, x.i))
	CASE(Q::QSz,  Db::push_ent(in->var, x.z))
	CASE(Q::QFlt, Db::push_ent(in->var, x.f))
	CASE(Q::QDbl, Db::push_ent(in->var, x.d))
	CASE(Q::QINT, Db::push_ent(in->var, x.iA))
	CASE(Q::QSZ,  Db::push_ent(in->var, x.zA))
	CASE(Q::QFLT, Db::push_ent(in->var, x.fA))
	CASE(Q::QDBL, Db::push_ent(in->var, x.dA))
	default: fatal("cannot store Q of type {}", (S)x.ty);
	}
}

inl auto load_Q(std::vector<Q::Q> *s, Bc::In *in) -> Q::Q {
	Q::Q r;
	std::optional<Db::Ent*> res;
	Db::Ent *ent;

	res = Db::get(in->var);
	if (!res) return r;

	ent = *res;
	switch (ent->ty) {
	CASE(Db::Int, r = Q::Q(ent->i))
	CASE(Db::Sz,  r = Q::Q(ent->z))
	CASE(Db::Flt, r = Q::Q(ent->f))
	CASE(Db::Dbl, r = Q::Q(ent->d))
	CASE(Db::INT, r = Q::Q(ent->iA))
	CASE(Db::SZ,  r = Q::Q(ent->zA))
	CASE(Db::FLT, r = Q::Q(ent->fA))
	CASE(Db::DBL, r = Q::Q(ent->dA))
	default: fatal("cannot load Db::Ent of type {}", (S)ent->ty);
	}

	return r;
}

inl auto add(std::vector<Q::Q> *s) -> char* {
	Q::Q r;

	auto x = stk_pop(s);
	auto y = stk_pop(s);
	auto T = Q::mkQTy_dyad(x.ty, y.ty);

	switch (T) {
	/* scalar adds */
	CASE(Q::mkQTy_dyad(Q::QInt, Q::QInt), r = Q::Q(x.i + y.i))
	CASE(Q::mkQTy_dyad(Q::QSz,  Q::QSz),  r = Q::Q(x.z + y.z))
	CASE(Q::mkQTy_dyad(Q::QFlt, Q::QFlt), r = Q::Q(x.f + y.f))
	CASE(Q::mkQTy_dyad(Q::QDbl, Q::QDbl), r = Q::Q(x.d + y.d))

	default:
		return A_err(
			"'nyi: {}+{}",
			Q::QTy_short[x.ty], Q::QTy_short[y.ty]
		);
	}

	s->push_back(r);
	return nullptr;
}

inl auto mkAf64(std::vector<Q::Q> *s) -> char* {
	auto r = stk_pop(s).to_S();
	if (!r) return A_err("mkAf64(): {}", r.error());
	auto L = *r;
	auto a = A::A<f64>(L);

	for (S i = 0; i < L; i++) {
		auto x = stk_pop(s).to_f64();
		if (!r) return A_err("mkAf64(): {}", r.error());
		a[i] = *x;
	}

	auto q = Q::Q(a);
	std::cout << Fmt::Fmt(&q) << std::endl;
	s->push_back(q);

	return nullptr;
}

inl auto exe_in(std::vector<Q::Q> *s, Bc::In *in) -> char* {
	char *err;

	switch (in->ty) {
	/* literals */
	CASE(Bc::LITi32, s->push_back(Q::Q(in->i)))
	CASE(Bc::LITSz,  s->push_back(Q::Q(in->z)))
	CASE(Bc::LITf32, s->push_back(Q::Q(in->f)))
	CASE(Bc::LITf64, s->push_back(Q::Q(in->d)))

	/* stack ops */
	CASE(Bc::POP,    s->pop_back());
	CASE(Bc::STORE,  store_Q(s, in))
	CASE(Bc::LOAD,   s->push_back(load_Q(s, in)))

	/* vector ops */
	CASE(Bc::MKAf64, if ((err = mkAf64(s))) return err)

	/* arithmetic */
	CASE(Bc::ADD, if ((err = add(s))) return err)

	default: return A_err("unknown instruction of type {}", (S)in->ty);
	}
	return nullptr;
}

auto Asm::Asm::exe_body(S body) -> R<Q::Q> {
	i64 i;
	Bc::In *in;
	VM::Bod *b;
	std::vector<Q::Q> stk;
	char *err;

	b = &bods[body];

	for (
		i = b->start, in = ins+i;
		i < inL && !in->is(Bc::RET);
		i++, in = ins+i
	) {
		if ((err = exe_in(&stk, in))) {
			return err_fmt("'exe: {}", err);
		}
	}

	i = stk.size() - 1;
	return i >= 0 ? stk[i] : Q::Q();
}
