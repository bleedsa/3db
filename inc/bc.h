#ifndef __3DB_BC_H__
#define __3DB_BC_H__

#include <str.h>
#include <sse.h>
#include <Q.h>
#include <var.h>
#include <A.h>
#include <T.h>

namespace Bc {
	enum InTy {
		/* 0 = RETURN */
		RET,
		/* push literal objs */
		LITi32,  /* (i)       -- Q                 | create lit i32 */
		LITf64,  /* (d)       -- Q                 | create lit f64 */
		LITChr,  /* (c)       -- Q                 | create lit char */
		LITVar,  /* (v)       -- Q                 | create lit var */
		/* stack/var ops */
		SHOW,    /* ( ) x     -- cout<<stk         | show the stack */
		POP,     /* ( ) x     --                   | pop off the stack */
		LOAD,    /* (x)       -- vars[x]           | load var at x */
		STORE,   /* (x) y     -- vars[x]:y         | store y at var x */
		DEL,     /* (x)       -- drop vars[x]      | delete var x */
		/* vector ops */
		MKAi32,  /* ( ) i xs  -- vec i long        | make a vector */
		MKAf64,  /* ( ) i xs  -- vec i long        | make a vector */
		MKAChr,  /* ( ) i xs  -- vec i long        | make a vector */
		GETVEC,  /* ( ) x i   -- x[i]              | get elem from vec */
		/* table ops */
		MKT,     /* (x)   t*x -- tab with cols t   | new table with X */
		TINSERT, /* (x)   insert into tab at row x |                  */
		TSETCELL,/* (x,y) x   -- set cell at (x,y) |                  */
		/* arithmetic */
		ADD,     /* ( ) x y   -- x+y               | addition */
		SUB,     /* ( ) x y   -- x-y               | subtraction*/
		MUL,     /* ( ) x y   -- x*y               | multiplication */
		DIV,     /* ( ) x y   -- x/y               | division */
	};

	struct In {
		InTy ty;
		union {
			I i;
			f64 d;
			char c;
			var_t var;
			xmm64_t x64;
		};

		inl In() : ty{RET} {}
		inl In(InTy ty) : ty{ty} {}

		In(InTy ty, I x);
		In(InTy ty, f64 x);
		In(InTy ty, char x);
		In(InTy ty, const char *x);
		In(InTy ty, var_t x);
		In(InTy ty, T::TColTy x);
		In(InTy ty, xmm64_t x64);

		In(I x);
		In(f64 x);
		In(char x);
		In(var_t x);
		In(const char *x);

		inl ~In() {}

		In(const In &x);
		const In &operator=(const In &x);

		inl bool is(InTy t) {
			return ty == t;
		}
	};
}

#endif
