#include <T.h>

int main() {
	Three::init();

	auto t = T::T(
		A::A{"ints", "dbls"},
		A::A{T::TInt, T::TDbl}
	);
	t.insert(000, 123, 4.56);
	t.insert(330, 456, 7.89);
	t.insert(001, 1,   2.00);
	t.insert(016, 789, 1.23);
	std::cout << Fmt::Fmt(&t) << std::endl;

	auto v = T::T(
		A::A{"int vec", "dbl vec", "chr vec"},
		A::A{T::TINT,   T::TDBL,   T::TCHR}
	);
	auto i = A::A<i32>{1, 2, 3, 4, 5};
	auto d = A::A<f64>{1.2, 3.4, 5.6};
	auto c = A::A<Chr>{'a', 'b', 'c'};
	for (S x = 0; x < 16; x++) {
		v.insert(x, i.ptr, i.len, d.ptr, d.len, c.ptr, c.len);
	}
	std::cout << Fmt::Fmt(&v) << std::endl;

	Three::deinit();
}
