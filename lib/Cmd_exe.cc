#include <utility>

#include <cmd.h>

template<typename X>
inl auto set_col(T::T *t, S row, S col, X x) -> void {
	((X*)t->cols[col])[row] = x;
}

inl auto insert_exe(Cmd::Insert *x, T::T *t) -> R<void> {
	S col;
	Q::Q *q;

	/* safety */
	[[unlikely]]
	if (t->coln != x->cols.size()) {
		auto n = var_to_str(x->name);
		auto r = err_fmt(
			"attempting to insert {} columns into table with"
			"{} columns.",
			x->cols.size(), t->coln
		);
		delete[] n;
		return r;
	}

	/* set each column */
	for (
		col = 0, q = &x->cols[col];
		col < x->cols.size();
		col++, q = &x->cols[col]
	) {
		switch (q->ty) {
		[[unlikely]]
		CASE(Q::QNil, {
			auto n = var_to_str(x->name);
			auto r = err_fmt("attempting to insert nil into {}", n);
			delete[] n;
			return r;
		})

		CASE(Q::QInt, set_col<i32>(t, x->row, col, q->i))
		CASE(Q::QDbl, set_col<f64>(t, x->row, col, q->d))
		CASE(Q::QChr, set_col<Chr>(t, x->row, col, q->c))
		CASE(Q::QINT, set_col<A::A<i32>>(t, x->row, col, q->iA))
		CASE(Q::QDBL, set_col<A::A<f64>>(t, x->row, col, q->dA))
		CASE(Q::QCHR, set_col<A::A<Chr>>(t, x->row, col, q->cA))

		default: std::unreachable();
		}
	};

	/* set the row as init */
	t->init[x->row] = true;

	return {};
}

inl auto create_table(
	var_t name,
	std::optional<std::vector<Cmd::Col>> &c
) -> Db::Ent* {
	if (c) {
		auto cols = *c;
		auto [names, types] = U::unzip<var_t, T::TColTy>(cols);
		auto n = A::from_vec(names);
		auto t = A::from_vec(types);
		return Db::add(name, T::T(n, t));
	} else {
		return Db::add(name, T::empty());
	}
}

inl auto set_ent(Db::Ent *ent, Q::Q val) -> R<MaybePtr<Db::Ent>> {
	switch (val.ty) {
	CASE(Q::QInt, ent->ty = Db::Int, ent->i = val.i)
	CASE(Q::QDbl, ent->ty = Db::Dbl, ent->d = val.d)
	CASE(Q::QChr, ent->ty = Db::Ch,  ent->c = val.c)
	CASE(Q::QINT, ent->ty = Db::INT, ent->iA = val.iA)
	CASE(Q::QDBL, ent->ty = Db::DBL, ent->dA = val.dA)
	CASE(Q::QCHR, ent->ty = Db::CHR, ent->cA = val.cA)
	}
	return MaybePtr(ent);
}

/* full selection of columns has a hot path that uses raw memcpy for atomic
 * columns. for selections with a where clause, see sel_where above. */
auto sel(Db::Ent *e, T::T *t, Cmd::Select *s) -> R<MaybePtr<Db::Ent>> {
	auto names = &s->cols;
	auto coln = names->len;
	auto cols = mk<u8*>(coln);
	auto tys = A::A<T::TColTy>(coln);
	auto init = mk<bool>(t->row_cap);

	/* copy a vector column into cols */
	auto vec_col = [&]<typename X>(S x, S orig) {
		for (S y = 0; y < t->row_cap; y++) if (t->init[y]) {
			auto cell = t->get_cell<X>(orig, y);
			auto new_cell = ((A::A<X>*)cols[x])+y;
			*new_cell = *cell;
		}
	};

	/* make the columns matrix */
	try {
		for (S i = 0; i < coln; i++) {
			/* find the column */
			auto x = t->find_col_idx(names->at(i));
			/* populate the type vector */
			tys[i] = t->col_tys[x];
			/* calculate the col size */
			S z = T::TColTy_Z[tys[i]] * t->row_cap;
			/* allocate */
			cols[i] = mk<u8>(z);
			/* copy */
			if (t->col_is_vec(x)) switch (tys[i]) {
			CASE(T::TINT, CALL(vec_col, i32, i, x))
			CASE(T::TDBL, CALL(vec_col, f64, i, x))
			CASE(T::TCHR, CALL(vec_col, Chr, i, x))
			} else memmove(cols[i], t->cols[x], z);
		}
	} catch (std::string &e) {
		return std::unexpected(e);
	}

	if (s->where) {
		auto w = *s->where;
		for (S y = 0; y < t->row_cap; y++) 
			init[y] = t->init[y] && w(t, y);
	} else {
		memmove(init, t->init, t->row_cap);
	}

	/* make a table out of all the stuff we just did */
	auto r = T::T(*names, tys, cols, t->row_cap, init);
	return MaybePtr(Db::Ent(e->name, r));
}

auto Cmd::Cmd::exe() -> R<MaybePtr<Db::Ent>> {
	switch (ty) {
	case SELECT: {
		auto o = Db::get(select.name);
		if (!o) {
			auto n = var_to_str(select.name);
			auto r = err_fmt("table {} not found.", n);
			delete[] n;
			return r;
		}

		/* check if we got back a table */
		auto e = *o;
		if (e->ty != Db::Tab) {
			auto n = var_to_str(select.name);
			auto r = err_fmt("entry {} is not a table.", n);
			delete[] n;
			return r;
		}

		auto t = e->as_T();

		return sel(e, t, &select);
	}

	case INSERT: {
		auto o = Db::get(insert.name);
		if (!o) {
			auto n = var_to_str(insert.name);
			auto r = err_fmt("table {} not found.", n);
			delete[] n;
			return r;
		}

		/* check if we got back a table */
		auto e = *o;
		if (e->ty != Db::Tab) {
			auto n = var_to_str(insert.name);
			auto r = err_fmt("entry {} is not a table.", n);
			delete[] n;
			return r;
		}

		/* grab the table and make room for the row */
		auto t = e->as_T();
		t->reZ(insert.row);
		
		auto r = insert_exe(&insert, t);
		if (!r) return std::unexpected(r.error());

		return MaybePtr(e);
	}

	case CREATE: {
		Db::Ent *r;
		auto n = create.name;
		auto o = Db::get(n);
		if (o) r = *o;
		else switch (create.ty) {
		CASE(Db::Int, r = Db::add(n, 0))
		CASE(Db::Dbl, r = Db::add(n, 0.0))
		CASE(Db::Ch,  r = Db::add(n, '\0'))
		CASE(Db::INT, r = Db::add(n, A::A<i32>(0)))
		CASE(Db::DBL, r = Db::add(n, A::A<f64>(0)))
		CASE(Db::CHR, r = Db::add(n, A::A<Chr>(0)))
		[[likely]]
		CASE(Db::Tab, r = create_table(n, create.cols))
		}
		return MaybePtr(r);
	}

	case GET: {
		auto n = get.name;
		auto o = Db::get(n);
		if (o) [[likely]] return MaybePtr(*o);
		else return err_fmt("entry {} not found", var_to_str(n));
	}

	case SET: {
		auto n = set.name;
		auto q = set.val;
		auto o = Db::get(n);
		if (o) [[likely]] return set_ent(*o, q);
		else return err_fmt("entry {} not found", var_to_str(n));
	}

	default: return err_fmt("cannot exe Cmd of type {}", type_name());
	}
}
