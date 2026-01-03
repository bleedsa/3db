#include <Asm.h>

inl auto exe_in(std::vector<Q::Q> *s, Bc::In *in) -> bool {
	switch (in->ty) {
	CASE(Bc::LITf32, s->push_back(Q::Q(in->f));)
	default: return false;
	}
	return true;
}

auto Asm::Asm::exe_body(S body) -> R<Q::Q> {
	S i;
	Bc::In *in;
	VM::Bod *b;
	std::vector<Q::Q> stk;
	bool res;
	
	b = &bods[body];

	for (i = b->start, in = ins+i; i < inL && !in->is(Bc::RET); i++, in = ins+i) {
		std::cout << (S)in->ty << std::endl;
		if (!(res = exe_in(&stk, in))) {
			return std::unexpected("failed to execute instruction");
		}
	}

	auto r = stk[stk.size() - 1];
	return r;
}
