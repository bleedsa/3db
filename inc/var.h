#ifndef __3DB_VAR_H__
#define __3DB_VAR_H__

#include <cstring>

#include <mem.h>

typedef char var_t __attribute__((vector_size(16))); 
#define VARZ (Z(var_t))

var_t str_to_var(const char *y);
char *var_to_str(var_t y);

#endif
