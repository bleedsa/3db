#include <u.h>
#include <db.h>

int main() {
	Three::init();

	auto path = "o/run/test.db";

	/* make a table */
	auto t = T::T(
		A::A{"ints", "dbls", "strs"},
		A::A{T::TInt, T::TDbl, T::TCHR}
	);
	auto a = A::A<Chr>{'a', 'b', 'c', 'd', 'e', 'f'};
	t.insert(01, 1, 1.23, a.ptr, a.len);
	t.insert(02, 2, 4.56, a.ptr, a.len);
	t.insert(03, 3, 7.89, a.ptr, a.len);

	/* push some entries */
	Db::push_ent("x", A::A<i32>{1, 2, 3, 4, 5});
	Db::push_ent("y", A::A<f64>{1.0, 2.0, 3.0});
	Db::push_ent("z", t);

	/* write */
	Db::write(path);

	/* reset the ents and load */
	Db::ents = std::vector<Db::Ent>();
	Db::load(path);

	{
		auto G = LOCK(Db::mut);
		for (auto &e : Db::ents) std::cout << Fmt::Fmt(&e) << std::endl;
	}

	Three::deinit();
}
