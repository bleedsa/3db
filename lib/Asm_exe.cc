#include <Asm.h>

inl auto exe_in(std::vector<Q::Q> *s, Bc::In *in) -> char* {
	switch (in->ty) {
	CASE(Bc::LITf32, s->push_back(Q::Q(in->f));)
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
