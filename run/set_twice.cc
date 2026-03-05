#include <db.h>

int main() {
	Three::init();

	auto v = "var";
	Db::add(v, 1);
	Db::add(v, 2);

	auto o = Db::get(v);
	if (!o) std::cout << "not found" << std::endl;
	else {
		auto e = *o;
		std::cout << Fmt::Fmt(e) << std::endl;
	}

	Three::deinit();
}
