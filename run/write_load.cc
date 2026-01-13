#include <u.h>
#include <db.h>

int main() {
	Three::init();

	auto path = "o/run/test.db";

	Db::push_ent("x", A::A<i32>{1, 2, 3, 4, 5});
	Db::push_ent("y", A::A<f64>{1.0, 2.0, 3.0});
	Db::write(path);

	Db::ents = std::vector<Db::Ent>();
	Db::load(path);

	{
		auto G = LOCK(Db::mut);
		for (auto &e : Db::ents) std::cout << Fmt::Fmt(&e) << std::endl;
	}

	Three::deinit();
}
