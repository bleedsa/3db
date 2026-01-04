#include <var.h>

auto var_to_str(var_t y) -> char* {
	auto x = mk<char>(Z(var_t));
	memcpy(x, &y, Z(var_t));
	return x;
}
