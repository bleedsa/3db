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

	Three::deinit();
}
