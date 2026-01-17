#include <ini.h>

int main() {
	Three::init();

	INI::INI ini;
	char *err;

	if ((err = ini.parse("run/cfg.ini"))) {
		std::cout << err << std::endl;
		return -1;
	}

	for (auto &[k, v] : ini.map) {
		std::cout << k << ' ' << v << std::endl;
	}

	Three::deinit();
}
