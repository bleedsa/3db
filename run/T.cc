#include <T.h>

int main() {
	Three::init();

	auto t = T::T(A::A{"ints", "dbls"}, A::A{T::TInt, T::TDbl});
	t.insert(000, 123, 4.56);
	t.insert(330, 456, 7.89);
	t.insert(001, 1,   2.0);

	std::cout << Fmt::Fmt(&t) << std::endl;

	Three::deinit();
}
