#include <cstdarg>

#include <str.h>
#include <T.h>

S T::TColTy_Z[] = {
	[TInt]=Z(i32), [TDbl]=Z(f64), [TChr]=Z(Chr),
	[TINT]=Z(A::A<i32>), [TDBL]=Z(A::A<f64>), [TCHR]=Z(A::A<Chr>),
};

const char *T::TColTy_short[] = {
	[TInt]="i", [TDbl]="d", [TChr]="c",
	[TINT]="I", [TDBL]="D", [TCHR]="C",
};

const char *T::TColTy_names[] = {
	[TInt]="i32", [TDbl]="f64", [TChr]="chr",
	[TINT]="I32", [TDBL]="F64", [TCHR]="CHR",
};

bool T::TColTy_is_vec[] = {
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
	auto ptr = get_cell<u8>(x, y);
	ptr->~A();
}

auto T::T::free_cells() -> void {
	for (S x = 0; x < coln; x++) { 
		for (S y = 0; y < row_cap; y++) {
			if (init[y]&&col_is_vec(x)) free_vec_cell(x, y);
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
	auto a = t->alloc_cell<X>(len, x, y);
	memmove(a->ptr, arg, Z(X)*len);
}

auto T::T::insert(S id, ...) -> char* {
	va_list args;
	va_start(args, id);

	reZ(id);
	init[id] = true;

	for (S i = 0; i < coln; i++) {
		switch (col_tys[i]) {
		/* basic setter for atomic types */
		#define set_col(Y) \
			auto arg = va_arg(args, Y);((Y*)cols[i])[id] = arg;
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


template<typename X>
inl auto cln_col(T::T *t, u8 *ptr, u8 **cols, S x) -> void {
	for (S y = 0; y < t->row_cap; y++) if (t->init[y]) {
		auto a = t->get_cell<X>(x, y);
		auto L = a->len;
		auto N = A::A<X>(L);
		memmove(N.ptr, a->ptr, Z(X)*L);
		((A::A<X>*)ptr)[y] = N;
	}
}

auto T::T::cln() -> void {
	/* make a bunch of pointers */
	auto col_tys = mk<TColTy>(coln);
	auto col_names = mk<var_t>(coln);
	auto cols = mk<u8*>(coln);
	auto init = mk<bool>(row_cap);
	auto refs = new i64;

	/* copy everything over */
	memmove(col_tys,   this->col_tys,   Z(TColTy)*coln);
	memmove(col_names, this->col_names, Z(var_t)*coln);
	memmove(init,      this->init,      Z(bool)*row_cap);
	*refs = *this->refs;

	/* fill all the columns */
	for (S x = 0; x < coln; x++) {
		auto ptr = mk<u8>(colZof(x));
		auto col = this->cols[x];

		switch (col_tys[x]) {
		/* atoms are memmove */
		CASE(TInt, memmove(ptr, col, Z(i32)*row_cap))
		CASE(TDbl, memmove(ptr, col, Z(f64)*row_cap))
		CASE(TChr, memmove(ptr, col, Z(Chr)*row_cap))

		/* vecs are their own thing */
		CASE(TINT,cln_col<i32>(this, ptr, cols, x))
		CASE(TDBL,cln_col<f64>(this, ptr, cols, x))
		CASE(TCHR,cln_col<Chr>(this, ptr, cols, x))

		default: fatal("can't cln T with column of {}", col_type(x));
		}

		cols[x] = ptr;
	}

	this->col_tys=col_tys, this->col_names=col_names, this->cols=cols;
	this->init=init,       this->refs=refs;
}

