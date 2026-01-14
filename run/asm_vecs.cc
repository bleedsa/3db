#include <Asm.h>

template<typename X>
inl auto f(A::A<X> v, Bc::InTy ty) -> void {
	auto a = Asm::Asm();
	a.push_bod(VM::Bod(0, 0));

	for (S i = 0; i < v.len; i++) {
		a.push_in(v[i]);
	}
	a.push_in((I)v.len);
	a.push_in(ty);

	auto r = a.exe();
	if (r) {
		auto x = *r;
		std::cout << Fmt::Fmt(&x) << std::endl;
	} else {
		std::cout << r.error() << std::endl;
	}
}

int main() {
	Three::init();

	f(A::A<i32>{1, 2, 3}, Bc::MKAi32);
	f(A::A<f64>{1.234, 5.678}, Bc::MKAf64);
	f(A::A<Chr>{'a', 'b', 'c'}, Bc::MKAChr);

	Three::deinit();
}
