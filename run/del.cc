#include <db.h>
#include <Asm.h>

int main() {
	Three::init();

	auto v = str_to_var("x");
	auto a = Asm::Asm();
	a.push_bod(VM::Bod(0, 0));
	a.push_in(3);
	a.push_in(Bc::In(Bc::STORE, v));
	a.push_in(Bc::In(Bc::DEL, v));
	a.push_in(Bc::In(Bc::LOAD, v));
	a.push_in(Bc::In());

	auto r = a.exe();
	if (r) {
		auto q = *r;
		std::cout << Fmt::Fmt(&q) << std::endl;
	} else {
		std::cout << r.error() << std::endl;
	}

	Three::deinit();
}
