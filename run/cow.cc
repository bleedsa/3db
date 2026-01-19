#include <db.h>
#include <T.h>

int main() {
	auto x = T::T(
		A::A{"ints", "dbls", "int vec"},
		A::A{T::TInt, T::TDbl, T::TINT}
	);
	auto i = A::A<i32>{1, 2, 3, 4, 5, 6, 7};
	x.insert(01, 1, 1.2, i.ptr, i.len);
	x.insert(02, 2, 3.4, i.ptr, i.len);
	x.insert(05, 3, 5.6, i.ptr, i.len);
	x.insert(10, 4, 7.8, i.ptr, i.len);
	x.insert(12, 5, 9.0, i.ptr, i.len);

	auto y = x;
	y.set_cell<i32>(0, 10, 0);

	i = A::A{9, 8, 7, 6, 5, 4, 3, 2, 1, 0};
	y.set_cell<i32>(2, 10, i.ptr, i.len);

	std::cout << Fmt::Fmt(&x) << std::endl;
	std::cout << Fmt::Fmt(&y) << std::endl;
}
