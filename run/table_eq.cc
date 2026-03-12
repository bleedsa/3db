#include <T.h>

int main() {
	Three::init();

	auto f = [&]() {
		auto t = T::T(
			A::A{"ints", "dbls"},
			A::A{T::TInt, T::TDbl}
		);

		t.insert(001, 1, 1.23);
		t.insert(010, 2, 4.56);
		t.insert(100, 3, 7.89);

		return t;
	};

	auto x = f();
	auto y = f();
	auto z = T::empty();

	std::cout << (x == y) << (x == z) << std::endl;

	Three::deinit();
}
