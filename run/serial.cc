#include <db.h>

#ifndef NAT
__static_yoink("__die");
#endif

template<typename X>
inl auto f(const char *n, X a) -> void {
	Db::push_ent(n, a);
	auto e = *Db::get(n);

	std::cout << "wrote entry " << Fmt::Fmt(e) << std::endl;

	auto [B, L] = e->to_bytes();
	for (S i = 0; i < L; i++) std::cout << 44 + B[i];
	std::cout << std::endl;

	*e = Db::Ent(B);
	std::cout << "deserialized entry " << Fmt::Fmt(e) << std::endl;
	free(B);
};

int main() {
	Three::init();

	f("x", A::A<i32>{1, 2, 3});
	f("y", A::A<f64>{1.2, 3.4});
	f("z", (S)1234567890);

	Three::deinit();
}
