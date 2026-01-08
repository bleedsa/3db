#include <cassert>

#include <var.h>

auto str_to_var(const char *y) -> var_t {
	auto len = strlen(y);
	assert(len > 0);

	auto ptr = mk<char>(VARZ);
	auto L = std::min(VARZ, len+1);

	memcpy(ptr, y, L);
	memset(ptr+L, 0, VARZ-L);
	ptr[L - 1] = 0;

	return *(var_t*)ptr;
}

auto var_to_str(var_t y) -> char* {
	auto x = mk<char>(Z(var_t));
	memcpy(x, &y, Z(var_t));
	return x;
}
