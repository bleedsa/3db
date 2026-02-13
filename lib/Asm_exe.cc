#include <Asm.h>
#include <db.h>

inl auto stk_pop(std::vector<Q::Q> *s) -> Q::Q {
	i64 i = s->size() - 1;
	if (i >= 0) {
		auto r = s->at(i);
		s->pop_back();
		return r;
	} else {
		return Q::Q();
	}
}

inl auto stk_pop_head(std::vector<Q::Q> *s) -> Q::Q {
	auto r = s->at(0);
	s->erase(s->begin());
	return r;
}

inl auto stk_head(std::vector<Q::Q> *s) -> Q::Q* {
	return &s->at(0);
}

inl auto stk_show(std::vector<Q::Q> *s) -> void {
	for (auto &x : *s) {
		std::cout << x.short_name() << ' ';
		std::cout << Fmt::Fmt(&x) << std::endl;
	}
}


inl auto store_Q(std::vector<Q::Q> *s, Bc::In *in) -> char* {
	auto x = stk_pop(s);

	switch (x.ty) {
	CASE(Q::QInt, Db::push_ent(in->var, x.i))
	CASE(Q::QDbl, Db::push_ent(in->var, x.d))
	CASE(Q::QChr, Db::push_ent(in->var, x.c))
	CASE(Q::QINT, Db::push_ent(in->var, x.iA))
	CASE(Q::QDBL, Db::push_ent(in->var, x.dA))
	CASE(Q::QCHR, Db::push_ent(in->var, x.cA))
	CASE(Q::QTab, Db::push_ent(in->var, x.t))
	default: return A_err("cannot store Q of type {}", x.short_name());
	}

	return nullptr;
}

inl auto load_Q(std::vector<Q::Q> *s, Bc::In *in) -> char* {
	Q::Q r;
	std::optional<Db::Ent*> res;
	Db::Ent *ent;

	res = Db::get(in->var);
	if (!res) {
		auto v = var_to_str(in->var);
		auto e = A_err("entry {} not found", v);
		delete[] v;
		return e;
	}

	ent = *res;
	switch (ent->ty) {
	CASE(Db::Int, r = Q::Q(ent->i))
	CASE(Db::Dbl, r = Q::Q(ent->d))
	CASE(Db::Ch,  r = Q::Q(ent->c))
	CASE(Db::INT, r = Q::Q(ent->iA))
	CASE(Db::DBL, r = Q::Q(ent->dA))
	CASE(Db::CHR, r = Q::Q(ent->cA))
	CASE(Db::Tab, r = Q::Q(ent->t))
	default: return A_err("cannot load entry of type {}", ent->type());
	}

	s->push_back(r);
	return nullptr;
}

inl auto del(std::vector<Q::Q> *s, Bc::In *in) -> char* {
	char *err;
	if ((err = Db::del(in->var))) return err;
	return nullptr;
}

inl auto add(std::vector<Q::Q> *s) -> char* {
	Q::Q r;

	auto x = stk_pop(s);
	auto y = stk_pop(s);
	auto T = Q::mkQTy_dyad(x.ty, y.ty);

	switch (T) {
	/* scalar adds */
	CASE(Q::mkQTy_dyad(Q::QInt, Q::QInt), r = Q::Q(x.i + y.i))
	CASE(Q::mkQTy_dyad(Q::QDbl, Q::QDbl), r = Q::Q(x.d + y.d))

	default: return A_err("'nyi: {}+{}", x.short_name(), y.short_name());
	}

	s->push_back(r);
	return nullptr;
}

inl auto mkAi32(std::vector<Q::Q> *s) -> char* {
	auto r = stk_pop(s).to_S();
	if (!r) return A_err("mkAi32(): {}", r.error());
	auto L = *r;
	auto a = A::A<i32>(L);

	for (S i = 0; i < L; i++) {
		auto x = stk_pop(s).to_i32();
		if (!x) return A_err("mkAi32(): {}", r.error());
		a[i] = *x;
	}

	s->push_back(Q::Q(a));
	return nullptr;
}

inl auto mkAf64(std::vector<Q::Q> *s) -> char* {
	auto r = stk_pop(s).to_S();
	if (!r) return A_err("mkAf64(): {}", r.error());
	auto L = *r;
	auto a = A::A<f64>(L);

	for (S i = 0; i < L; i++) {
		auto x = stk_pop(s).to_f64();
		if (!x) return A_err("mkAf64(): {}", r.error());
		a[i] = *x;
	}

	s->push_back(Q::Q(a));
	return nullptr;
}

inl auto mkAchr(std::vector<Q::Q> *s) -> char* {
	auto r = stk_pop(s).to_S();
	if (!r) return A_err("mkAchr(): (S)x: {}", r.error());
	auto L = *r;
	auto a = A::A<Chr>(L);

	for (S i = 0; i < L; i++) {
		auto x = stk_pop(s).to_chr();
		if (!x) return A_err("mkAchr(): (Chr)x: {}", r.error());
		a[i] = *x;
	}

	s->push_back(Q::Q(a));
	return nullptr;
}

inl auto mkT(std::vector<Q::Q> *s, Bc::In *in) -> char* {
	auto coln = (S)in->i;
	auto tys = A::A<T::TColTy>(coln);
	auto names = A::A<var_t>(coln);

	for (S i = 0; i < coln; i++) {
		auto r = stk_pop(s).to_i32();
		if (!r) return A_err("mkT(): {}", r.error());
		tys[i] = (T::TColTy)*r;

		auto v = stk_pop(s).to_var();
		if (!v) return (char*)v.error().c_str();
		names[i] = *v;
	}

	s->push_back(Q::Q(T::T(names, tys)));

	return nullptr;
}

template<typename X>
inl auto insert_col_vec(T::T *t, S x, S y, A::A<X> a) -> void {
	auto ptr = t->get_cell<X>(x, y);
	*ptr = a;
}

inl auto Tinsert(std::vector<Q::Q> *s, Bc::In *in) -> char* {
	S i, id = (S)in->i;

	/* get a ptr to the table */
	auto r = stk_head(s)->to_T_ptr();
	if (!r) {
		std::stringstream ss;
		ss << "Tinsert(): bottom of the stack is not a table.";
		ss << std::endl;

		for (auto &x : *s) {
			ss << x.short_name() << ' ';
			ss << Fmt::Fmt(&x) << std::endl;
		}

		u8 *p = Str::Interns::ptr[Str::Interns::add(ss.str().c_str())];
		return (char*)p;

	}

	auto t = *r;
	t->reZ(id);

	for (i = 0; i < t->coln; i++) {
		auto x = stk_pop(s);
		switch (t->col_tys[i]) {
		/* atoms */
		#define set_col_atom(X, f) { \
			auto R=f(); \
			if (R) ((X*)t->cols[i])[id]=*R; \
			else return A_err("'cast: {}", R.error()); \
		}
		CASE(T::TInt, set_col_atom(i32, x.to_i32))
		CASE(T::TDbl, set_col_atom(f64, x.to_f64))
		CASE(T::TChr, set_col_atom(Chr, x.to_chr))

		/* vectors */
		CASE(T::TINT, insert_col_vec<i32>(t, i, id, x.iA))
		CASE(T::TDBL, insert_col_vec<f64>(t, i, id, x.dA))
		CASE(T::TCHR, insert_col_vec<Chr>(t, i, id, x.cA))

		default: return A_err(
			"cannot insert column of type {}", x.short_name()
		);
		}

	}
	t->init[id] = true;

	return nullptr;
}

template<typename X>
inl auto atom_set_cell(T::T *t, S x, S y, X v) -> char* {
	auto ptr = ((X*)t->cols[x])+y;
	*ptr = v;
	return nullptr;
}

template<typename X>
inl auto vec_set_cell(T::T *t, S x, S y, A::A<X> v) -> char* {
	auto len = v.len;
	auto a = t->alloc_cell<X>(len, x, y);
	*a = v;
	return nullptr;
}

inl auto Tsetcell(std::vector<Q::Q> *s, Bc::In *in) -> char* {
	/* get a ptr to the table */
	auto r = stk_head(s)->to_T_ptr();
	if (!r) {
		std::stringstream ss;
		ss << "Tsetcell(): bottom of the stack is not a table.";
		ss << std::endl;

		for (auto &x : *s) {
			ss << x.short_name() << ' ';
			ss << Fmt::Fmt(&x) << std::endl;
		}

		u8 *p = Str::Interns::ptr[Str::Interns::add(ss.str().c_str())];
		return (char*)p;

	}

	auto t = *r;
	auto [x, y] = SSE::xmm64_unfuse(in->x64);
	auto q = stk_pop(s);

	switch (q.ty) {
	/* atom set cells */
	CASE(Q::QInt, return atom_set_cell<i32>(t, x, y, q.i))
	CASE(Q::QDbl, return atom_set_cell<f64>(t, x, y, q.d))
	CASE(Q::QChr, return atom_set_cell<Chr>(t, x, y, q.c))

	/* vector set cells */
	CASE(Q::QINT, return vec_set_cell<i32>(t, x, y, q.iA))
	CASE(Q::QDBL, return vec_set_cell<f64>(t, x, y, q.dA))
	CASE(Q::QCHR, return vec_set_cell<Chr>(t, x, y, q.cA))

	default: return A_err("cannot set cell of type {}", q.short_name());
	}

	return nullptr;
}

inl auto exe_in(std::vector<Q::Q> *s, Bc::In *in) -> char* {
	char *err;

	switch (in->ty) {
	/* literals */
	CASE(Bc::LITi32, s->push_back(Q::Q(in->i)))
	CASE(Bc::LITf64, s->push_back(Q::Q(in->d)))
	CASE(Bc::LITChr, s->push_back(Q::Q(in->c)))
	CASE(Bc::LITVar, s->push_back(Q::Q(in->var)))

	/* stack ops */
	CASE(Bc::SHOW,   stk_show(s))
	CASE(Bc::POP,    s->pop_back());

	/* db ops */
	CASE(Bc::STORE,  if ((err = store_Q(s, in))) return err)
	CASE(Bc::LOAD,   if ((err = load_Q(s, in))) return err)
	CASE(Bc::DEL,    del(s, in))

	/* vector ops */
	CASE(Bc::MKAi32, if ((err = mkAi32(s))) return err)
	CASE(Bc::MKAf64, if ((err = mkAf64(s))) return err)
	CASE(Bc::MKAChr, if ((err = mkAchr(s))) return err)

	/* table ops */
	CASE(Bc::MKT, if ((err = mkT(s, in))) return err)
	CASE(Bc::TINSERT, if ((err = Tinsert(s, in))) return err)
	CASE(Bc::TSETCELL, if ((err = Tsetcell(s, in))) return err)

	/* arithmetic */
	CASE(Bc::ADD, if ((err = add(s))) return err)

	default: return A_err("unknown instruction of type {}", (S)in->ty);
	}
	return nullptr;
}

auto Asm::Asm::exe_body(S body) -> R<Q::Q> {
	i64 i;
	Bc::In *in;
	VM::Bod *b;
	std::vector<Q::Q> stk;
	char *err;

	b = &bods[body];

	for (
		i = b->start, in = ins+i;
		i < inL && !in->is(Bc::RET);
		i++, in = ins+i
	) {
		if ((err = exe_in(&stk, in))) {
			return err_fmt("'exe: {}", err);
		}
	}

	i = stk.size() - 1;
	return i >= 0 ? stk[i] : Q::Q();
}
