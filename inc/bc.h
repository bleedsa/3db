#ifndef __3DB_BC_H__
#define __3DB_BC_H__

#include <str.h>

namespace Bc {
	enum InTy {
		LOAD,   /* (x)   -- vars[x]           | load var at x */
		STORE,  /* (x) y -- vars[x]:y         | store y at var x */
		MKTAB,  /* (x)   -- vars[x]:empty tab | new table at x */
		GETCOL, /* (x) y -- col of vars[x]@y  | get col y of x */
	};

	struct In {
		InTy ty;
		union {
			Str::A str;
		};

		In(InTy ty, Str::A str);
		~In();

		In(const In &x);
		const In &operator=(const In &x);
	};
}

#endif
