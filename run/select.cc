#include <Asm.h>

int main() {
	Three::init();

	auto n = str_to_var("ints");
	auto a = Asm::Asm();
	a.push_bod(VM::Bod(0, 0));

	/* make a simple table */
	a.push_in("ints");
	a.push_in(T::TInt);
	a.push_in(Bc::In(Bc::MKT, 1));

	/* push some ints */
	auto is = A::A{1, 2, 3, 4, 5, 6, 7};
	auto i = 0;
	is.for_each([&](i32 *x){
		a.push_in(*x);
		a.push_in(Bc::In(Bc::TINSERT, i));
		i++;
	});

	/* select */

	Three::deinit();
}
