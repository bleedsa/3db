#include <u.h>
#include <str.h>
#include <db.h>

int main() {
	Three::init();

	Db::push_ent("0", 1234);
	Db::push_ent("1", (S)5678);
	Db::push_ent("2", 1234.5f);
	Db::push_ent("3", 1234.56);

	for (auto &e : Db::ents) {
		std::cout << Fmt::Ent(&e) << std::endl;
	}

	Three::deinit();
}
