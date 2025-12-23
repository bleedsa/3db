#ifndef __3DB_STR_H__
#define __3DB_STR_H__

#include <mutex>

#include <u.h>

namespace Str {
	namespace Interns {
		extern u8 **ptr;
		extern S cap, len;
		extern std::mutex mut;

		void init();
		void deinit();
		void reZ();

		S push(u8 *str);
		S add(u8 *str);
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
