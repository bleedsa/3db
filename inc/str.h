#ifndef __3DB_STR_H__
#define __3DB_STR_H__

#include <mutex>

#include <u.h>

namespace Str {
	namespace Interns {
		extern std::mutex mut;
		/* the matrix of strings */
		extern u8 **ptr;
		/* static allocation tracking */
		extern S cap, len;
		/* refcounting */
		extern S *refs;
		/* allocs */
		extern S refs_cap, refs_len;

		void init();
		void deinit();
		void reZ();

		S push(u8 *str);
		S add(u8 *str);
		S add(const char *str);
	}

	/* static sized, heap allocated string */
	struct A {
		S len;
		S str;

		A(const char *str);
		~A();

		A(const A &x);
		const A &operator=(const A &x);

		char *cstr();
	};
}

#endif
