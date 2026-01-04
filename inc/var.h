#ifndef __3DB_VAR_H__
#define __3DB_VAR_H__

#include <mem.h>

typedef char var_t __attribute__((vector_size(16))); 
#define VARZ (Z(var_t))

inl auto str_to_var(const char *y) -> var_t {
	auto ptr = mk<char>(VARZ);
	auto L = std::min(VARZ, strlen(y)+1);
	memcpy(ptr, y, L);
	memset(ptr+L, 0, VARZ-L);
	ptr[L] = 0;
	return *(var_t*)ptr;
}

char *var_to_str(var_t y);

#endif
