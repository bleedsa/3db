#include <iostream>

#include <Asm.h>

void mv(Asm::Asm x) {
	std::cout << x.inL << ' ' << x.bodL << std::endl;
}

int main() {
	auto a = Asm::Asm();
	for (i32 i = 0; i < 64; i++) a.push_in(Bc::In(i));
	for (S i = 0; i < 128; i++) a.push_bod(VM::Bod(0, i));
	auto b = a;
	mv(b);
	return 0;
}
