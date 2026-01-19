#include <Asm.h>

int main() {
	Three::init();

	auto a = Asm::Asm();
	a.push_bod(VM::Bod(0, 0));

	/* make a table */
	a.push_in("x");
	a.push_in(T::TInt);
	a.push_in("y");
	a.push_in(T::TDbl);
	a.push_in("z");
	a.push_in(T::TCHR);
	a.push_in(Bc::In(Bc::MKT, 3));

	/* insert a row */
	a.push_in(12345);
	a.push_in(678.9);
	a.push_in('z');
	a.push_in('y');
	a.push_in('x');
	a.push_in(3);
	a.push_in(Bc::MKAChr);
	a.push_in(Bc::In(Bc::TINSERT, 0));

	/* update the row with a new f64 */
	a.push_in(1.2345);
	a.push_in(Bc::In(Bc::TSETCELL, SSE::xmm64_fuse(1, 0)));

	/* update the row with a new str */
	a.push_in('a');
	a.push_in('b');
	a.push_in('c');
	a.push_in(3);
	a.push_in(Bc::MKAChr);
	a.push_in(Bc::In(Bc::TSETCELL, SSE::xmm64_fuse(0, 0)));

	auto r = a.exe();
	if (r) {
		auto q = *r;
		std::cout << Fmt::Fmt(&q) << std::endl;
	} else {
		std::cout << r.error() << std::endl;
	}

	Three::deinit();
}
