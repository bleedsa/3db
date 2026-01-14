#include <T.h>

int main() {
	Three::init();

	auto t = T::T(
		A::A{"ints", "dbls", "chrs"},
		A::A{T::TInt, T::TDbl, T::TChr}
	);
	t.insert(000, 123, 4.56, 'a');
	t.insert(330, 456, 7.89, 'b');
	t.insert(001, 1,   2.0,  'c');
	std::cout << Fmt::Fmt(&t) << std::endl;

	auto v = T::T(
		A::A{"int vec", "dbl vec", "chr vec"},
		A::A{T::TINT,   T::TDBL,   T::TCHR}
	);
	auto i = A::A<i32>{1, 2, 3, 4, 5};
	auto d = A::A<f64>{1.0, 2.0, 3.0};
	auto c = A::A<Chr>{'a', 'b', 'c'};
	v.insert(0, i.ptr, i.len, d.ptr, d.len, c.ptr, c.len);
	std::cout << Fmt::Fmt(&v) << std::endl;

	Three::deinit();
}
