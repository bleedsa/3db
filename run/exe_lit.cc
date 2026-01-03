#include <Asm.h>

int main() {
	/* make a dummy asm */
	auto a = Asm::Asm();
	a.push_in(Bc::In(11111.1111f));
	a.push_in(Bc::In());
	a.push_bod(VM::Bod(0, 0));

	/* exe */
	auto r = a.exe_body(0);
	if (!r) {
		std::cerr << r.error() << std::endl;
		return -1;
	}
	auto q = *r;
	std::cout << Fmt::Q(&q) << std::endl;
	return 0;
}
