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
	extern bool TColTy_is_vec[6];

	struct T {
		u32 coln, row_cap;
		TColTy *col_tys;
		var_t *col_names;
		u8 **cols;
		bool *init;
		i64 *refs;

		T(A::A<var_t> n, A::A<TColTy> a);

		inl T(A::A<const char*> n, A::A<TColTy> a) {
			auto v = n.each<var_t>([](const char **n) {
				return str_to_var(*n);
			});
			*this = T(v, a);
		}

		void free_vec_cell(S x, S y);
		void free_cells();
		~T();

		T(const T &x);
		const T &operator=(const T &x);

		inl auto col_type(S i) -> const char* {
			return TColTy_names[col_tys[i]];
		}

		inl auto col_is_vec(S i) -> bool {
			return TColTy_is_vec[col_tys[i]];
		}

		inl auto colZof(S i) -> S {
			return TColTy_Z[col_tys[i]] * row_cap;
		}

		inl auto row_is_init(S i) -> bool {
			return init[i];
		}

		template<typename X>
		inl auto calc_vec_col_Z(S x) -> S {
			S z = 0;
			for (S y = 0; y < row_cap; y++) {
				if (init[y]) {
					auto [ptr, buf, len] = get_cell<X>(x, y);
					z += Z(u64) + (Z(X) * len);
				}
			}
			return z;
		}

		/* calculate the size of the buffer needed to serialize this
		 * table into bytes. */
		inl auto calc_serial_Z() -> S {
			S z = 0;
			for (S i = 0; i < coln; i++) {
				switch (col_tys[i]) {
				case TInt:
				case TDbl:
				case TChr:
					z += colZof(i);
					break;
				
				/* vecs are a little more complicated */
				CASE(TINT, z += calc_vec_col_Z<i32>(i))
				CASE(TDBL, z += calc_vec_col_Z<f64>(i))
				CASE(TCHR, z += calc_vec_col_Z<Chr>(i))
				}
			}
			return z
				+ (Z(bool) * row_cap)
				+ (Z(TColTy) * coln)
				+ (Z(var_t) * coln);
		}

		/* get a cell as an A::A. NOT memory safe */
		template<typename X>
		inl auto cell_to_A(S x, S y) -> A::A<X> {
			auto ptr = ((S**)cols[x])[y];
			auto buf = (X*)(ptr+1);
			auto len = *ptr;
			return A::A<X>(buf, len);
		}

		void reZ(S row);
		char *insert(S id, ...);
		char *insert_col(S id, S col, u8 *ptr, S z);

		void fill_buf(u8 *ptr);
		void from_buf(u8 *ptr);

		inl void from_buf_full(u8 *ptr) {
			memcpy(&coln,    ptr, Z(u32));   ptr += Z(u32);
			memcpy(&row_cap, ptr, Z(u32));   ptr += Z(u32);
			from_buf(ptr);
		}

		inl void fill_buf_full(u8 *ptr) {
			memcpy(ptr, &coln,    Z(u32));   ptr += Z(u32);
			memcpy(ptr, &row_cap, Z(u32));   ptr += Z(u32);
			fill_buf(ptr);
		}

		template<typename X>
		inl auto get_cell(S x, S y) -> std::tuple<S*, X*, S> {
			auto ptr = ((S**)cols[x])[y];
			auto buf = (X*)(ptr+1);
			auto len = *ptr;
			return {ptr, buf, len};
		}

		template<typename X>
		inl auto alloc_cell(S L, S x, S y) -> std::tuple<S*, X*> {
			auto ptr = (S*)mk<u8>(Z(S) + (Z(X) * L));
			auto buf = (X*)(ptr+1);
			*ptr = L;
			((S**)cols[x])[y] = ptr;
			return {ptr, buf};
		}

		template<typename X>
		inl auto set_cell(S x, S y, X v) -> void {
			if (col_is_vec(x)) {
				std::cout << "bad set_cell for column of type ";
				std::cout << col_type(x);
				exit(-1);
			} else {
				auto ptr = ((X*)cols[x])+y;
				memmove(ptr, &v, Z(X));
			}
		}

		template<typename X>
		inl auto set_cell(S x, S y, X *P, S L) -> void {
			if (col_is_vec(x)) {
				/* free the previous cell */ {
					auto [ptr, buf, len] = get_cell<X>(x, y);
					free(ptr);
				}

				/* allocate and set the new cell */
				auto [ptr, buf] = alloc_cell<X>(L, x, y);
				memmove(buf, P, Z(X)*L);
			} else {
				std::cout << "bad set_cell for column of type ";
				std::cout << col_type(x);
				exit(-1);
			}
		}
	};
}

namespace Fmt {
	std::string Fmt(T::T *t);
}

#endif
