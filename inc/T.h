#ifndef __3DB_T_H__
#define __3DB_T_H__

#include <u.h>
#include <A.h>
#include <var.h>

namespace T {
	enum TColTy {
		TInt,
		TDbl,
		TChr,
		TINT,
		TDBL,
		TCHR,
	};

	extern S TColTy_Z[6];
	extern const char *TColTy_short[6];
	extern const char *TColTy_names[6];

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

		inl auto alloc_row() -> u8* {
			/* calculate the buffer size */
			S *Zs = new S[coln], z;
			for (S i = 0; i < coln; i++) {
				Zs[i] = TColTy_Z[col_tys[i]];
			}
			z = U::sum_vec<S>(Zs, coln);

			/* alloc */
			auto ptr = new u8[z];
			return ptr;
		}

		void reZ(S row);
		char *insert(S id, ...);
		char *insert_col(S id, S col, u8 *ptr, S z);
	};
}

namespace Fmt {
	std::string Fmt(T::T *t);
}

#endif
