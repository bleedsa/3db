#include <u.h>
#include <str.h>
#include <db.h>

int main() {
	char *n;

	Three::init();

	for (S i = 0; i < 2222; i++) {
		asprintf(&n, ".%zu", i);
		Db::push_ent(n, i);
		free(n);
	}

	for (auto &e : Db::ents) {
		std::cout << var_to_str(e.name) << ' ' << e.z;
	}

	std::cout << std::endl;

	Three::deinit();
}
