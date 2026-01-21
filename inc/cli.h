#ifndef __3DB_CLI_H__
#define __3DB_CLI_H__

#include <u.h>
#include <Q.h>

namespace Cli {
	struct Cli {
		char *addr;

		Cli(const char *a);
		~Cli();

		R<Q::Q> set(var_t n, Q::Q *q);

		inl auto set(const char *n, Q::Q *q) -> R<Q::Q> {
			return set(str_to_var(n), q);
		}
	};
}

#endif
