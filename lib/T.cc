#include <cstdarg>

#include <str.h>
#include <T.h>

S T::TColTy_Z[] = {
	Z(i32), Z(f64), Z(Chr),
	Z(i32*), Z(f64*), Z(Chr*),
};

const char *T::TColTy_short[] = {
	"i", "d", "c",
	"I", "D", "C",
};

const char *T::TColTy_names[] = {
	"i32", "f64", "chr",
	"I32", "F64", "CHR",
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
	auto ptr = ((S**)cols[x])[y];
	free(ptr);
}

auto T::T::free_cells() -> void {
	for (S i = 0; i < coln; i++) { 
		switch (col_tys[i]) {
		CASE(TINT, for(S j=0;j<row_cap;j++) if (init[j]) free_vec_cell(i, j))
		CASE(TDBL, for(S j=0;j<row_cap;j++) if (init[j]) free_vec_cell(i, j))
		CASE(TCHR, for(S j=0;j<row_cap;j++) if (init[j]) free_vec_cell(i, j))
		default: {}
		}
		free(cols[i]);
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
/* setters for vecs expect a ptr and a length */
#define set_vec(Y) { \
	auto arg = va_arg(args, Y*); \
	auto len = va_arg(args, S); \
	auto ptr = mk<u8>(Z(S) + (Z(Y) * len)); \
	auto buf = (u8*)(((S*)ptr)+1); \
	*(S*)ptr = len; \
	memcpy(buf, arg, Z(Y)*len); \
	((u8**)cols[i])[id] = ptr; \
}
		CASE(TInt, set_col(i32))
		CASE(TDbl, set_col(f64))
		CASE(TChr, set_col(Chr))
		CASE(TINT, set_vec(i32))
		CASE(TDBL, set_vec(f64))
		CASE(TCHR, set_vec(Chr))
		default: return A_err(
			"cannot insert col of type {}", TColTy_short[col_tys[i]]
		);
		}
	}

	return nullptr;
}
