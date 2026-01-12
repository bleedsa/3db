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
	extern const S QTy_Z[9];

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

		Q(QTy ty, u8 *ptr, S L);

		~Q();

		Q(const Q &x);
		const Q &operator=(const Q &x);

		inl auto len() -> S {
			switch (ty) {
			/* 0=~nil */
			CASE(QNil, return 0)

			/* 1=~atom */
			case QInt:
			case QSz:
			case QFlt:
			case QDbl:
				return 1;

			/* get vector sizes */
			CASE(QINT, return iA.len)
			CASE(QSZ,  return zA.len)
			CASE(QFLT, return fA.len)
			CASE(QDBL, return dA.len)

			default: fatal("Q::len() on Q with type {}", (S)ty);
			}
		}

		inl auto atom_size() -> S {
			return QTy_Z[ty];
		}

		inl auto to_S() -> R<S> {
			switch (ty) {
			CASE(QInt, return (S)i)
			CASE(QSz,  return z)
			CASE(QFlt, return (S)f)
			CASE(QDbl, return (S)d)
			default: return err_fmt(
				"cannot convert Q of type {} to size", (S)ty
			);
			}
		}

		inl auto to_f64() -> R<f64> {
			switch (ty) {
			CASE(QInt, return (f64)i)
			CASE(QSz,  return (f64)z)
			CASE(QFlt, return (f64)f)
			CASE(QDbl, return d)
			default: return err_fmt(
				"cannot convert Q of type {} to f64", (S)ty
			);
			}
		}

		void fill_buf_with_elems(u8 *buf);
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
