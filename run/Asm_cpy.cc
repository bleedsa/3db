#include <iostream>

#include <Asm.h>

void mv(Asm::Asm x) {
	std::cout << x.inL << ' ' << x.bodL << std::endl;
}

int main() {
	auto a = Asm::Asm();
	for (S i = 0; i < 64; i++) a.push_in(Bc::In(i));
	auto b = a;
	mv(b);
	return 0;
}
