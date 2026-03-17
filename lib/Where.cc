#include <cmd.h>

auto Cmd::Where::name(const char *n) -> Where& {
	auto x = str_to_var(n);
	col = x;
	return *this;
}

auto Cmd::Where::value(Q::Q q) -> Where& {
	val = q;
	return *this;
}

auto Cmd::Where::operator()(T::T *t, S row) -> bool {
	auto get = [&]<typename X>(S x) {
		return ((X*)t->cols[x])[row];
	};

	try {
		auto x = t->find_col_idx(col);
		switch (ty) {
		case Eq: switch (t->col_tys[x]) {
			CASE(T::TInt, return val.i == CALL(get, i32, x))
			CASE(T::TDbl, return val.d == CALL(get, f64, x))
			CASE(T::TChr, return val.c == CALL(get, Chr, x))
			CASE(T::TINT, return val.iA == CALL(get, A::A<i32>, x))
			CASE(T::TDBL, return val.dA == CALL(get, A::A<f64>, x))
			CASE(T::TCHR, return val.cA == CALL(get, A::A<Chr>, x))
		}

		case Gt: switch (t->col_tys[x]) {
			CASE(T::TInt, return CALL(get, i32, x) > val.i)
			CASE(T::TDbl, return CALL(get, f64, x) > val.d)
			CASE(T::TChr, return CALL(get, Chr, x) > val.c)
			default: goto neg;
		}

		case Lt: switch (t->col_tys[x]) {
			CASE(T::TInt, return CALL(get, i32, x) < val.i)
			CASE(T::TDbl, return CALL(get, f64, x) < val.d)
			CASE(T::TChr, return CALL(get, Chr, x) < val.c)
			default: goto neg;
		}
		}
	} catch (std::string &e) {
		return false;
	}

neg:
	return false;
}
