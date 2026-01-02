#include <u.h>
#include <str.h>
#include <db.h>

int main() {
	char *n;

	Three::init();

	for (S i = 0; i < 22222; i++) {
		asprintf(&n, ".%zu", i);
		Db::push_ent(n, i);
		free(n);
	}

	for (auto &e : Db::ents) {
		std::cout << e.name << ' ' << e.z;
	}

	std::cout << std::endl;

	Three::deinit();
}
