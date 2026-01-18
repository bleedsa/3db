#include <ini.h>

#include "cfg.h"

Cfg::Cfg(const char *path) {
	auto ini = INI::INI(path);

	file = ini["file"], port = ini["port"];
}
