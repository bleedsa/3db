#ifndef __3DB_Q_H__
#define __3DB_Q_H__

#include <string>

#include <u.h>
#include <A.h>

namespace Q {
	enum QTy {
		QNil,
		QInt,
		QSz,
		QFlt,
		QDbl,
		QINT,
		QSZ,
		QFLT,
		QDBL,
	};

	extern const char *QTy_short[9];

	struct Q {
		QTy ty;
		union {
			i32 i;
			S z;
			f32 f;
			f64 d;
			A::A<i32> iA;
			A::A<S> zA;
			A::A<f32> fA;
			A::A<f64> dA;
		};

		inl Q()      : ty{QNil} {}
		inl Q(i32 x) : ty{QInt}, i{x} {}
		inl Q(S x)   : ty{QSz},  z{x} {}
		inl Q(f32 x) : ty{QFlt}, f{x} {}
		inl Q(f64 x) : ty{QDbl}, d{x} {}

		inl Q(A::A<i32> x) : ty{QINT}, iA{x} {}
		inl Q(A::A<S> x)   : ty{QSZ},  zA{x} {}
		inl Q(A::A<f32> x) : ty{QFLT}, fA{x} {}
		inl Q(A::A<f64> x) : ty{QDBL}, dA{x} {}

		~Q();

		Q(const Q &x);
		const Q &operator=(const Q &x);
	};

	/* make a "dyad" identifier (two (u8)Qty mashed together) */
	constexpr auto mkQTy_dyad(QTy x, QTy y) -> u16 {
		return (x << 8) | y;
	}
}

namespace Fmt {
	std::string Fmt(Q::Q *x);
}

#endif
