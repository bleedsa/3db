#include <u.h>
#include <str.h>
#include <db.h>

int main() {
	Three::init();

	Db::add("0", 1234);
	Db::add("2", 1234.5f);
	Db::add("3", 1234.56);

	for (auto &e : Db::ents) {
		std::cout << Fmt::Fmt(&e) << std::endl;
	}

	Three::deinit();
}
