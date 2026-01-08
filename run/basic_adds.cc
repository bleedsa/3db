#include <Asm.h>

template<typename X, typename Y>
inl auto run_add(X x, Y y) -> void {
	auto a = Asm::Asm();
	a.push_bod(VM::Bod(0, 0));
	a.push_in(Bc::In(x));
	a.push_in(Bc::In(y));
	a.push_in(Bc::In(Bc::ADD));
	a.push_in(Bc::In());

	auto r = a.exe();
	if (!r) {
		std::cerr << r.error() << std::endl;
		return;
	}

	auto q = *r;
	std::cout << Fmt::Q(&q) << std::endl;
}

int main() {
	Three::init();

	run_add(1234, 5678);
	run_add((S)1, (S)2);
	run_add(1.2f, 3.4f);
	run_add(1.23, 4.56);

	Three::deinit();
}
