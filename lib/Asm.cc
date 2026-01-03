#include <Asm.h>
#include <mem.h>

/* empty with caps of 16 */
Asm::Asm::Asm()
	: start{0}, inL{0}, in_cap{16}, bodL{0}, bod_cap{16}
	, ins{mk<Bc::In>(in_cap)}, bods{mk<VM::Bod>(bod_cap)}
{}

/* copy Asm y into x */
inl auto Asm_cpy(Asm::Asm *x, Asm::Asm *y) -> void {
	/* copy counters */
	x->inL=y->inL,   x->in_cap=y->in_cap;
	x->bodL=y->bodL, x->bod_cap=y->bod_cap;

	/* copy values */
	x->ins = mk<Bc::In>(x->in_cap), x->bods = mk<VM::Bod>(x->bod_cap);
	memcpy(x->ins,  y->ins,  Z(Bc::In)*y->inL);
	memcpy(x->bods, y->bods, Z(VM::Bod)*y->bodL);
}

Asm::Asm::Asm(const Asm &x) {
	Asm_cpy((Asm*)this, (Asm*)&x);
}

auto Asm::Asm::operator=(const Asm &x) -> const Asm& {
	Asm_cpy((Asm*)this, (Asm*)&x);
	return *this;
}

Asm::Asm::~Asm() {
	free(ins);
	free(bods);
}

/* resize buffers if the len exceeds the cap */
auto Asm::Asm::reZ() -> void {
	if (inL >= in_cap) {
		in_cap *= 2;
		ins = remk<Bc::In>(ins, in_cap);
	}
	if (bodL >= bod_cap) {
		bod_cap *= 2;
		bods = remk<VM::Bod>(bods, bod_cap);
	}
}

auto Asm::Asm::push_in(Bc::In x) -> void {
	reZ();
	/* memmove for simd */
	memmove(ins+inL++, (void*)&x, Z(Bc::In));
}

auto Asm::Asm::push_bod(VM::Bod x) -> void {
	reZ();
	memmove(bods+bodL++, (void*)&x, Z(VM::Bod));
}
