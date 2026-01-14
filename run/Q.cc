#include <u.h>
#include <Q.h>

int main() {
	Three::init();

	auto x2 = Q::Q(1234.560);
	auto x3 = Q::Q(12345678);
	Q::Q vec[] = {x2, x3};

	for (auto &x : vec) {
		std::cout << Fmt::Fmt(&x) << ' ';
	}

	std::cout << std::endl;

	Three::deinit();
}
