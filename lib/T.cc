#include <cstdarg>

#include <str.h>
#include <T.h>

S T::TColTy_Z[] = {
	Z(i32), Z(f64), Z(Chr),
	Z(A::A<i32>), Z(A::A<f64>), Z(A::A<Chr>),
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
	: coln{a.len}, row_cap{8}
	, col_tys{new TColTy[coln]}
	, col_names{new var_t[coln]}
	, cols{new u8*[coln]}
	, init{mk<bool>(row_cap)}
	, refs{new S}
{
	*refs = 1;
	/* just rip the pointer directly from the vecs */
	memcpy(col_tys, a.ptr, Z(TColTy)*coln);
	memcpy(col_names, n.ptr, Z(var_t)*coln);
	memset(init, false, Z(bool)*row_cap);
	/* allocate buffers for the columns */
	for (S i = 0; i < coln; i++) cols[i] = mk<u8>(colZof(i));
}

T::T::~T() {
	if (0 == --refs) {
		for (S i = 0; i < coln; i++) free(cols[i]);
		delete[] cols;
		delete[] col_tys;
		free(init);
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
		S prev_cap = row_cap;
		row_cap = row + 1;
		init = remk<bool>(init, row_cap);
		memset(init+prev_cap, false, Z(bool)*(row_cap - prev_cap));

		for (S i = 0; i < coln; i++) {
			switch (col_tys[i]) {
#define REMK(X) cols[i] = (u8*)remk<X>((X*)cols[i], row_cap)
			CASE(TInt, REMK(i32))
			CASE(TDbl, REMK(f64))
			CASE(TChr, REMK(Chr))
			CASE(TINT, REMK(i32*))
			CASE(TDBL, REMK(f64*))
			CASE(TCHR, REMK(Chr*))
			default: fatal("reZ on T of type {}", TColTy_short[i]);
			}
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

//auto T::T::insert_
