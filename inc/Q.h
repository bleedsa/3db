#ifndef __3DB_Q_H__
#define __3DB_Q_H__

#include <string>

#include <u.h>

namespace Q {
	enum QTy {
		QNil,
		QInt,
		QSz,
		QFlt,
		QDbl,
	};

	struct Q {
		QTy ty;
		union {
			i32 i;
			S z;
			f32 f;
			f64 d;
		};

		inl Q()      : ty{QNil} {}
		inl Q(i32 x) : ty{QInt}, i{x} {}
		inl Q(S x)   : ty{QSz},  z{x} {}
		inl Q(f32 x) : ty{QFlt}, f{x} {}
		inl Q(f64 x) : ty{QDbl}, d{x} {}

		inl ~Q() {}

		Q(const Q &x);
		const Q &operator=(const Q &x);
	};
}

namespace Fmt {
	std::string Q(Q::Q *x);
}

#endif
