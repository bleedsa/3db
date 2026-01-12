#include <u.h>
#include <A.h>

int main() {
	Three::init();

	auto x = A::A<int>{1, 2, 3};
	auto y = x;

	std::cout << Fmt::Fmt(&y) << std::endl;

	auto z = A::A<f32>{1.0f, 2.0f, 3.0f};
	std::cout << Fmt::Fmt(&z) << std::endl;

	Three::deinit();
}
