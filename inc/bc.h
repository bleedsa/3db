#ifndef __3DB_BC_H__
#define __3DB_BC_H__

#include <str.h>
#include <Q.h>
#include <var.h>

namespace Bc {
	enum InTy {
		RET,
		/* push literal objs */
		LITi32, /* (i)     -- Q                 | create literal i32 */
		LITSz,  /* (z)     -- Q                 | create literal S */
		LITf32, /* (f)     -- Q                 | create literal f32 */
		LITf64, /* (d)     -- Q                 | create literal f64 */
		/* stack/var ops */
		POP,    /* ( ) x   --                   | pop off the stack */
		LOAD,   /* (x)     -- vars[x]           | load var at x */
		STORE,  /* (x) y   -- vars[x]:y         | store y at var x */
		/* table ops */
		MKTAB,  /* (x)     -- vars[x]:empty tab | new table at x */
		GETCOL, /* (x) y   -- col of vars[x]@y  | get col y of x */
		/* arithmetic */
		ADD,    /* ( ) x y -- x+y               | addition */
		SUB,    /* ( ) x y -- x-y               | subtraction*/
		MUL,    /* ( ) x y -- x*y               | multiplication */
		DIV,    /* ( ) x y -- x/y               | division */
	};

	struct In {
		InTy ty;
		union {
			I i;
			S z;
			f32 f;
			f64 d;
			var_t var;
		};

		inl In() : ty{RET} {}
		inl In(InTy ty) : ty{ty} {}

		In(InTy ty, I x);
		In(InTy ty, S x);
		In(InTy ty, f32 x);
		In(InTy ty, f64 x);
		In(InTy ty, const char *x);
		In(InTy ty, var_t x);
		In(I x);
		In(S x);
		In(f32 x);
		In(f64 x);
		inl ~In() {}

		In(const In &x);
		const In &operator=(const In &x);

		inl bool is(InTy t) {
			return ty == t;
		}
	};
}

#endif
