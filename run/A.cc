#include <u.h>
#include <A.h>

int main() {
	Three::init();

	auto x = A::A<int>(3);
	x[0] = 1;
	x[1] = 2;
	x[2] = 3;

	std::cout << Fmt::A(&x) << std::endl;

	Three::deinit();
}
