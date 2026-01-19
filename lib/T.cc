#include <cstdarg>

#include <str.h>
#include <T.h>

S T::TColTy_Z[] = {
	Z(i32), Z(f64), Z(Chr),
	Z(i32*), Z(f64*), Z(Chr*),
};

const char *T::TColTy_short[] = {
	[TInt]="i", [TDbl]="d", [TChr]="c",
	[TINT]="I", [TDBL]="D", [TCHR]="C",
};

const char *T::TColTy_names[] = {
	[TInt]="i32", [TDbl]="f64", [TChr]="chr",
	[TINT]="I32", [TDBL]="F64", [TCHR]="CHR",
};

bool T::TColTy_free_cell[] = {
	[TInt]=false, [TDbl]=false, [TChr]=false,
	[TINT]=true,  [TDBL]=true,  [TCHR]=true,
};

T::T::T(A::A<var_t> n, A::A<TColTy> a)
	: coln{(u32)a.len}, row_cap{8}
	, col_tys{mk<TColTy>(coln)}
	, col_names{mk<var_t>(coln)}
	, cols{mk<u8*>(coln)}
	, init{mk<bool>(row_cap)}
	, refs{new i64}
{
	*refs = 1;

	/* just rip the pointer directly from the vecs */
	memcpy(col_tys, a.ptr, Z(TColTy)*coln);
	memcpy(col_names, n.ptr, Z(var_t)*coln);
	memset(init, false, Z(bool)*row_cap);

	/* allocate buffers for the columns */
	for (S i = 0; i < coln; i++) cols[i] = mk<u8>(colZof(i));
}

auto T::T::free_vec_cell(S x, S y) -> void {
	auto [ptr, buf, len] = get_cell<void>(x, y);
	free(ptr);
}

auto T::T::free_cells() -> void {
	for (S x = 0; x < coln; x++) { 
		for (S y = 0; y < row_cap; y++) {
			if (init[y]&&col_free_cell(x)) free_vec_cell(x, y);
		}
		free(cols[x]);
	}
}


T::T::~T() {
	*refs -= 1;
	if (*refs == 0) {
		free_cells();
		free(cols);
		free(col_tys);
		free(col_names);
		free(init);
		delete refs;
	}
}

T::T::T(const T &x)
	: coln{x.coln}, row_cap{x.row_cap}
	, col_tys{x.col_tys}, col_names{x.col_names}, cols{x.cols}
	, init{x.init}
	, refs{x.refs}
{
	*refs += 1;
}

auto T::T::operator=(const T &x) -> const T& {
	coln = x.coln, row_cap = x.row_cap;
	col_tys = x.col_tys, col_names = x.col_names, cols = x.cols;
	init = x.init;
	refs = x.refs;
	*refs += 1;
	return *this;
}

auto T::T::reZ(S row) -> void {
	if (row >= row_cap) {
		S prev_cap, i;

		prev_cap = row_cap, row_cap = row * 2;

		/* remk and set the init column */
		init = remk<bool>(init, row_cap);
		memset(init+prev_cap, false, Z(bool)*(row_cap - prev_cap));

		/* remk the columns */
		for (i = 0; i < coln; i++) {
			cols[i] = remk<u8>(cols[i], colZof(i));
		}
	}
}

template<typename X>
inl auto T_insert_vec_col(T::T *t, va_list args, S x, S y) -> void {
	auto arg = va_arg(args, X*);
	auto len = va_arg(args, S);
	auto [ptr, buf] = t->alloc_cell<X>(len, x, y);
	memmove(buf, arg, Z(X)*len);
}

auto T::T::insert(S id, ...) -> char* {
	va_list args;
	va_start(args, id);

	reZ(id);

	init[id] = true;
	for (S i = 0; i < coln; i++) {
		switch (col_tys[i]) {
		/* basic setter for atomic types */
		#define set_col(Y) { \
			auto arg = va_arg(args, Y); \
			((Y*)cols[i])[id] = arg; \
		}
		CASE(TInt, set_col(i32))
		CASE(TDbl, set_col(f64))
		CASE(TChr, set_col(Chr))
		/* setters for vecs expect a ptr and a length */
		CASE(TINT, T_insert_vec_col<i32>(this, args, i, id))
		CASE(TDBL, T_insert_vec_col<f64>(this, args, i, id))
		CASE(TCHR, T_insert_vec_col<Chr>(this, args, i, id))
		default: return A_err(
			"cannot insert col of type {}", TColTy_short[col_tys[i]]
		);
		}
	}

	return nullptr;
}
