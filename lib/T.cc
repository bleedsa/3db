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
	if (a.ptr) memcpy(col_tys, a.ptr, Z(TColTy)*coln);
	if (n.ptr) memcpy(col_names, n.ptr, Z(var_t)*coln);
	memset(init, false, Z(bool)*row_cap);

	/* allocate buffers for the columns */
	for (S i = 0; i < coln; i++) cols[i] = mk<u8>(colZof(i));
}

T::T::T(
	A::A<var_t> &names,
	A::A<TColTy> &tys,
	u8 **cols,
	u32 row_cap,
	bool *in
) 
	: coln{static_cast<u32>(names.len)}
	, row_cap{row_cap}
	, col_tys{mk<TColTy>(coln)}
	, col_names{mk<var_t>(coln)}
	, cols{cols}
	, init{mk<bool>(row_cap)}
	, refs{new i64}
{
	*refs = 1;

	/* copy the vecs */
	if (names.ptr) memcpy(col_names, names.ptr, Z(var_t)*coln);
	if (tys.ptr) memcpy(col_tys, tys.ptr, Z(TColTy)*coln);
	memcpy(init, in, Z(bool)*row_cap);
}

inl auto dump_cell(T::T *t, S x, S y) -> void {
	std::string f;
	switch (t->col_tys[x]) {
	CASE(T::TINT, f = Fmt::Fmt(t->get_cell<i32>(x, y)))
	CASE(T::TDBL, f = Fmt::Fmt(t->get_cell<f64>(x, y)))
	CASE(T::TCHR, f = Fmt::Fmt(t->get_cell<Chr>(x, y)))
	}
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
	if (--*refs == 0) {
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
		*a = N;
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

auto T::T::tys_eq(T &y) -> bool {
	return memeq(col_tys, y.col_tys, coln);
}

auto T::T::names_eq(T &y) -> bool {
	return memeq(col_names, y.col_names, coln);
}

auto T::T::init_eq(T &y) -> bool {
	return memeq(init, y.init, row_cap);
}

template<typename X>
inl auto col_eq(T::T &x, T::T &y, S c) {
	for (S r = 0; r < x.row_cap; r++) if (x.init[r]) {
		if (((X**)x.cols[c])[r] != ((X**)y.cols[c])[r]) return false;
	}
	return true;
}

auto T::T::cols_eq(T &y) -> bool {
	for (S c = 0; c < coln; c++) {
		switch (col_tys[c]) {
		CASE(TInt, if (!col_eq<i32>(*this, y, c)) return false)
		CASE(TDbl, if (!col_eq<f64>(*this, y, c)) return false)
		CASE(TChr, if (!col_eq<Chr>(*this, y, c)) return false)
		CASE(TINT, if (!col_eq<A::A<i32>>(*this, y, c)) return false)
		CASE(TDBL, if (!col_eq<A::A<f64>>(*this, y, c)) return false)
		CASE(TCHR, if (!col_eq<A::A<Chr>>(*this, y, c)) return false)
		}
	}
	return true;
}

auto operator==(T::T &x, T::T &y) -> bool {
	return x.coln == y.coln && x.row_cap == y.row_cap
		&& x.tys_eq(y) && x.names_eq(y) && x.init_eq(y)
		&& x.cols_eq(y);
}
