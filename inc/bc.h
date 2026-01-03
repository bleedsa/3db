#ifndef __3DB_BC_H__
#define __3DB_BC_H__

#include <str.h>
#include <Q.h>

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
		ADDi32, /* ( ) x y -- x+y               | add 2 i32 */
		ADDSz,  /* ( ) x y -- x+y               | add 2 S */
		ADDf32, /* ( ) x y -- x+y               | add 2 f32 */
		ADDf64, /* ( ) x y -- x+y               | add 2 f64 */
	};

	struct In {
		InTy ty;
		union {
			I i;
			S z;
			f32 f;
			f64 d;
		};

		inl In() : ty{RET} {}

		In(InTy ty, I x);
		In(InTy ty, S x);
		In(InTy ty, f32 x);
		In(InTy ty, f64 x);
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
