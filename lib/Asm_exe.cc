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
	default:
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
	}

	return r;
}

inl auto exe_in(std::vector<Q::Q> *s, Bc::In *in) -> char* {
	switch (in->ty) {
	CASE(Bc::LITf32, s->push_back(Q::Q(in->f)))
	CASE(Bc::STORE,  store_Q(s, in))
	CASE(Bc::LOAD,   s->push_back(load_Q(s, in)))
	default: return (char*)"unknown instruction";
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

	for (i = b->start, in = ins+i; i < inL && !in->is(Bc::RET); i++, in = ins+i) {
		if ((err = exe_in(&stk, in))) {
			return err_fmt("'exe: {}", err);
		}
	}

	i = stk.size() - 1;
	return i >= 0 ? stk[i] : Q::Q();
}
