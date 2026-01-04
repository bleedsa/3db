#include <Asm.h>

int main() {
	Three::init();

	/* make a dummy asm */
	auto a = Asm::Asm();
	a.push_bod(VM::Bod(0, 0));
	a.push_in(Bc::In(123.4f));
	a.push_in(Bc::In(Bc::STORE, "float1"));
	a.push_in(Bc::In(Bc::LOAD, "float1"));
	a.push_in(Bc::In());

	/* exe */
	auto r = a.exe();
	if (!r) {
		std::cerr << r.error() << std::endl;
		return -1;
	}

	/* fmt */
	auto q = *r;
	std::cout << Fmt::Q(&q) << std::endl;
	return 0;

	Three::deinit();
}
