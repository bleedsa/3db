#include <u.h>
#include <str.h>
#include <db.h>

int main() {
	Three::init();

	for (S i = 0; i < 2222; i++) {
		auto n = (std::stringstream() << i).str().c_str();
		Db::push_ent(n, i);
	}

	for (auto &e : Db::ents) {
		auto str = var_to_str(e.name);
		std::cout << str << ' ' << e.z << ' ';
		delete[] str;
	}

	std::cout << std::endl;

	Three::deinit();
}
