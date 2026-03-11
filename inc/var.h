#ifndef __3DB_VAR_H__
#define __3DB_VAR_H__

#include <cstring>

typedef char var_t __attribute__((vector_size(16))); 
#define VARZ (sizeof(var_t))

var_t str_to_var(const char *y);
char *var_to_str(var_t y);
bool vareq(var_t x, var_t y);

inline auto empty_var() -> var_t {
	return (var_t){0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
}

#endif
