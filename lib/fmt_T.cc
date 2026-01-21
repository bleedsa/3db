#include <vector>
#include <sstream>

#include <T.h>

/* format vector column */
template<typename Y>
inl auto fmt_vec_col(std::stringstream &ss, T::T *t, S x, S y) -> void {
	auto a = t->get_cell<Y>(x, y);
	ss << Fmt::Fmt(a);
}

auto Fmt::Fmt(T::T *t) -> std::string {
	S x, y, i, rown;
	std::string *row;
	auto coln = t->coln; 
	auto head = new std::string[coln+1];
	auto Ls = new S[coln+1];
	auto cells = std::vector<std::string*>();

	/* the id column */
	head[0] = "ID", Ls[0] = 2;

	/* format the column header with the name and type */
	for (i = 0; i < coln; i++) {
		auto n = var_to_str(t->col_names[i]);
		auto Y = T::TColTy_short[t->col_tys[i]];
		auto s = std::format("{}:{}", n, Y);
		head[i+1] = s;
		Ls[i+1] = s.size();
		delete[] n;
	}

	/* format each row */
	for (y = 0, rown = 0; y < t->row_cap; y++) if (t->init[y]) {
		rown++;
		row = new std::string[coln+1];

		/* iterate the columns */
		for (x = 0; x < coln; x++) {
			std::stringstream ss;

			switch (t->col_tys[x]) {
			/* format atomic column */
			#define fmt_col(Y) ss << ((Y*)t->cols[x])[y]
			CASE(T::TInt, fmt_col(i32))
			CASE(T::TDbl, fmt_col(f64))
			CASE(T::TChr, fmt_col(Chr))

			/* format vector columns */
			CASE(T::TINT, fmt_vec_col<i32>(ss, t, x, y))
			CASE(T::TDBL, fmt_vec_col<f64>(ss, t, x, y))
			CASE(T::TCHR, fmt_vec_col<Chr>(ss, t, x, y))

			default: ss << '{' << t->col_type(x) << "???}";
			}

			auto col = ss.str();
			Ls[x+1] = std::max(Ls[x+1], col.size());
			row[x+1] = col;
		}

		/* the id column */
		auto col = std::format("{}", y);
		Ls[0] = std::max(Ls[0], col.size());
		row[0] = col;

		cells.push_back(row);
	}

	/*
	 * make the final string
	 */
	std::stringstream ss;
	auto L_sum = U::sum_vec<S>(Ls, coln + 1);

	/* pad L chars with c */
	auto pad = [&](S L, char c) {
		for (S i = 0; i < L; i++) ss << c;
	};
	/* make a line */
	auto ln = [&]() {
		pad(L_sum + coln + 1, '=');
		ss << std::endl;
	};

	ln();

	/* header */
	ss << '|';
	for (i = 0; i < coln + 1; i++) {
		auto c = head[i];
		ss << c;
		pad(Ls[i] - c.size(), ' ');
		ss << '|';
	}
	ss << std::endl;

	ln();

	/* cells */
	y = 0;
	for (auto &r : cells) {
		ss << '|';
		for (x = 0; x < coln + 1; x++) {
			auto c = r[x];
			ss << c;
			pad(Ls[x] - c.size(), ' ');
			ss << '|';
		}
		ss << std::endl;
		y++;
	}

	delete[] Ls;
	delete[] head;
	for (auto &r : cells) delete[] r;
	return ss.str();
}
