#include <bc.h>
#include <Asm.h>

int main() {
	Three::init();

	auto a = Asm::Asm();
	a.push_bod(VM::Bod(0, 0));
	a.push_in("ints");
	a.push_in(T::TInt);
	a.push_in("chrs");
	a.push_in(T::TChr);
	a.push_in(Bc::In(Bc::MKT, 2));

	auto r = a.exe();
	if (r) {
		auto x = *r;
		std::cout << Fmt::Fmt(&x) << std::endl;
	} else {
		std::cout << r.error() << std::endl;
	}

	Three::deinit();
}
