#ifndef __3DB_T_H__
#define __3DB_T_H__

#include <u.h>
#include <A.h>
#include <var.h>

namespace T {
	enum TColTy {
		TInt,
		TSz,
		TFlt,
		TDbl,
		TChr,
		TINT,
		TSZ,
		TFLT,
		TDBL,
		TCHR,
	};

	extern S TColTy_Z[10];
	extern const char *TColTy_short[10];
	extern const char *TColTy_names[10];

	struct T {
		S coln, row_cap;
		TColTy *col_tys;
		var_t *col_names;
		u8 **cols;
		bool *init;
		S *refs;

		T(A::A<var_t> n, A::A<TColTy> a);

		inl T(A::A<const char*> n, A::A<TColTy> a) {
			auto v = n.each<var_t>([](const char **n) {
				return str_to_var(*n);
			});
			*this = T(v, a);
		}

		~T();

		T(const T &x);
		const T &operator=(const T &x);

		inl auto col_type(S i) -> const char* {
			return TColTy_names[col_tys[i]];
		}

		inl auto colZof(S i) -> S {
			return TColTy_Z[(S)col_tys[i]] * row_cap;
		}

		inl auto row_is_init(S i) -> bool {
			return init[i];
		}

		void reZ(S row);
		char *insert(S id, ...);
	};
}

namespace Fmt {
	std::string Fmt(T::T *t);
}

#endif
