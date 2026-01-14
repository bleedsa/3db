#include <bc.h>
#include <Asm.h>

int main() {
	Three::init();

	auto a = Asm::Asm();
	a.push_bod(VM::Bod(0, 0));

	/* make the table */
	a.push_in("ints");
	a.push_in(T::TInt);
	a.push_in("chrs");
	a.push_in(T::TChr);
	a.push_in("dbls");
	a.push_in(T::TDbl);
	a.push_in(Bc::In(Bc::MKT, 3));

	/* push some entries */
	a.push_in(1);
	a.push_in('a');
	a.push_in(123.456);
	a.push_in(Bc::In(Bc::TINSERT, 1));
	a.push_in(2);
	a.push_in('b');
	a.push_in(987.654);
	a.push_in(Bc::In(Bc::TINSERT, 2));
	a.push_in(Bc::In());

	auto r = a.exe();
	if (r) {
		auto x = *r;
		std::cout << Fmt::Fmt(&x) << std::endl;
	} else {
		std::cout << r.error() << std::endl;
	}

	Three::deinit();
}
