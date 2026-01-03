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

	/*
	switch (x.ty) {
	CASE(Q::QInt, Db::push_ent(str, x.i))
	CASE(Q::QSz,  Db::push_ent(str, x.z))
	CASE(Q::QFlt, Db::push_ent(str, x.f))
	CASE(Q::QDbl, Db::push_ent(str, x.d))
	default:
	}
	*/
}

inl auto exe_in(std::vector<Q::Q> *s, Bc::In *in) -> char* {
	switch (in->ty) {
	CASE(Bc::LITf32, s->push_back(Q::Q(in->f)))
	CASE(Bc::STORE,  store_Q(s, in))
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
