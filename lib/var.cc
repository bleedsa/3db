#include <cassert>

#include <var.h>

auto str_to_var(const char *y) -> var_t {
	char ptr[VARZ];

	auto len = strlen(y);
	assert(len > 0);

	auto L = std::min(VARZ, len+1);
	memcpy(ptr, y, L);
	memset(ptr+L, 0, VARZ-L);
	ptr[L - 1] = 0;

	auto r = *(var_t*)ptr;
	return r;
}

auto var_to_str(var_t y) -> char* {
	auto x = mk<char>(Z(var_t));
	memcpy(x, &y, Z(var_t));
	return x;
}
