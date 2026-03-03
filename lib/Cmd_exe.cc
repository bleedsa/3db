#include <cmd.h>

template<typename X>
inl auto set_col(T::T *t, S row, S col, X x) -> void {
	((X*)t->cols[col])[row] = x;
}

inl auto insert_exe(Cmd::Insert *x, T::T *t) -> R<Db::Ent*> {
	S col;
	Q::Q *q;

	[[unlikely]]
	if (t->coln != x->cols.len) {
		auto n = var_to_str(x->name);
		auto r = err_fmt(
			"attempting to insert {} columns into table with"
			"{} columns.",
			x->cols.len, t->coln
		);
		delete[] n;
		return r;
	}

	for (
		col = 0, q = &x->cols[col];
		col < x->cols.len;
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

	return {};

}

inl auto create_table(var_t name, std::optional<A::A<Cmd::Col>> &c)->Db::Ent* {
	if (c) {
		auto cols = *c;
		auto [names, types] = A::unzip<var_t, T::TColTy>(cols);
		auto t = T::T(names, types);
		return Db::add(name, t);
	} else {
		return Db::add(name, T::empty());
	}
}

auto Cmd::Cmd::exe() -> R<Db::Ent*> {
	switch (ty) {
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
		return insert_exe(&insert, t);
	}

	case CREATE: {
		auto n = create.name;
		auto o = Db::get(n);
		if (o) return *o;
		else switch (create.ty) {
		CASE(Db::Int, return Db::add(n, 0))
		CASE(Db::Dbl, return Db::add(n, 0.0))
		CASE(Db::Ch,  return Db::add(n, '\0'))
		CASE(Db::INT, return Db::add(n, A::A<i32>(0)))
		CASE(Db::DBL, return Db::add(n, A::A<f64>(0)))
		CASE(Db::CHR, return Db::add(n, A::A<Chr>(0)))
		CASE(Db::Tab, return create_table(n, create.cols))
		}
	}

	default: return err_fmt("cannot exe Cmd of type {}", (u8)ty);
	}
}
