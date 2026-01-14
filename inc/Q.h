#ifndef __3DB_Q_H__
#define __3DB_Q_H__

#include <string>

#include <u.h>
#include <A.h>
#include <T.h>

namespace Q {
	enum QTy {
		QNil,
		QInt,
		QDbl,
		QChr,
		QINT,
		QDBL,
		QCHR,
		QTab,
		QVar,
	};

	extern const char *QTy_short[9];
	extern const S QTy_Z[9];

	struct Q {
		QTy ty;
		union {
			i32 i;
			f64 d;
			Chr c;
			A::A<i32> iA;
			A::A<f64> dA;
			A::A<Chr> cA;
			T::T t;
			var_t var;
		};

		inl Q()        : ty{QNil}         {}
		inl Q(i32 x)   : ty{QInt}, i{x}   {}
		inl Q(f64 x)   : ty{QDbl}, d{x}   {}
		inl Q(Chr x)   : ty{QChr}, c{x}   {}
		inl Q(var_t x) : ty{QVar}, var{x} {}

		inl Q(A::A<i32> x)  : ty{QINT}, iA{x} {}
		inl Q(A::A<f64> x)  : ty{QDBL}, dA{x} {}
		inl Q(A::A<Chr> x)  : ty{QCHR}, cA{x} {}

		inl Q(T::T x) : ty{QTab}, t{x} {}

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
			case QDbl:
			case QChr:
			case QVar:
				return 1;

			/* get vector sizes */
			CASE(QINT, return iA.len)
			CASE(QDBL, return dA.len)
			CASE(QCHR, return cA.len)

			/* table len is row cap */
			CASE(QTab, return t.row_cap)

			default: fatal("Q::len() on Q with type {}", (S)ty);
			}
		}

		inl auto atom_size() -> S {
			return QTy_Z[ty];
		}

		inl auto short_name() -> const char* {
			return QTy_short[ty];
		}

		inl auto to_i32() -> R<i32> {
			switch (ty) {
			CASE(QInt, return i)
			CASE(QDbl, return (f64)d)
			CASE(QChr, return (char)c)
			default: return err_fmt(
				"cannot convert Q of type {} to i32", (S)ty
			);
			}
		}

		inl auto to_S() -> R<S> {
			switch (ty) {
			CASE(QInt, return (S)i)
			CASE(QDbl, return (S)d)
			CASE(QChr, return (S)c)
			default: return err_fmt(
				"cannot convert Q of type {} to size", (S)ty
			);
			}
		}

		inl auto to_f32() -> R<f32> {
			switch (ty) {
			CASE(QInt, return (f32)i)
			CASE(QDbl, return (f32)d)
			CASE(QChr, return (f32)c)
			default: return err_fmt(
				"cannot convert Q of type {} to f32", (S)ty
			);
			}
		}

		inl auto to_f64() -> R<f64> {
			switch (ty) {
			CASE(QInt, return (f64)i)
			CASE(QDbl, return d)
			CASE(QChr, return (f64)c)
			default: return err_fmt(
				"cannot convert Q of type {} to f64", (S)ty
			);
			}
		}

		inl auto to_chr() -> R<Chr> {
			switch (ty) {
			CASE(QInt, return (Chr)i)
			CASE(QDbl, return (Chr)d)
			CASE(QChr, return c)
			default: return err_fmt(
				"cannot convert Q of type {} to char", (S)ty
			);
			}
		}

		inl auto to_var() -> R<var_t> {
			switch (ty) {
			CASE(QVar, return var)
			default: return err_fmt(
				"cannot convert Q of type {} to var", (S)ty
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
