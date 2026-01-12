#include <db.h>

#ifndef NAT
__static_yoink("__die");
#endif

int main() {
	Three::init();

	Db::push_ent("x", A::A<i32>{1, 2, 3});
	auto e = *Db::get("x");

	std::cout << "wrote entry " << Fmt::Fmt(e) << std::endl;

	auto [B, L] = e->to_bytes();
	for (S i = 0; i < L; i++) std::cout << 44 + B[i];
	std::cout << std::endl;

	*e = Db::Ent(B);
	std::cout << "deserialized entry " << Fmt::Fmt(e) << std::endl;
	free(B);

	Three::deinit();
}
