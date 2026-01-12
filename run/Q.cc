#include <u.h>
#include <Q.h>

int main() {
	Three::init();

	auto x1 = Q::Q(1234.56f);
	auto x2 = Q::Q(1234.560);
	auto x3 = Q::Q(12345678);
	auto x4 = Q::Q((S)12345);
	Q::Q vec[] = {x1, x2, x3, x4};

	for (auto &x : vec) {
		std::cout << Fmt::Fmt(&x) << ' ';
	}

	std::cout << std::endl;

	Three::deinit();
}
