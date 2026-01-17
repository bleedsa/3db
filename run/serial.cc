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

	*e = Db::Ent(B);
	std::cout << "deserialized entry " << Fmt::Fmt(e) << std::endl;
	free(B);
};

int main() {
	Three::init();

	f("x", A::A<i32>{1, 2, 3});
	f("y", A::A<f64>{1.2, 3.4});
	f("z", (S)1234567890);
	f("chr", (Chr)'X');
	f("str", A::A<Chr>{'a', 'b', 'c'});

	auto t = T::T(
		A::A{"ints",  "dbls", "strs"},
		A::A{T::TInt, T::TDbl, T::TCHR}
	);
	auto a = A::A<Chr>{'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j'};
	t.insert(00,1,2.345,a.ptr,a.len);a=a.each<Chr>([](Chr*c){return*c+4;});
	t.insert(01,2,6.789,a.ptr,a.len);a=a.each<Chr>([](Chr*c){return*c+4;});
	t.insert(33,3,22.22,a.ptr,a.len);
	f("tab", t);

	Three::deinit();
}
