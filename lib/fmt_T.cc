#include <vector>
#include <sstream>

#include <T.h>

auto Fmt::Fmt(T::T *t) -> std::string {
	S x, y, i, rown;
	std::string *row;
	auto coln = t->coln; 
	auto head = new std::string[coln];
	auto Ls = new S[coln];
	auto cells = std::vector<std::string*>();

	/* format the column header with the name and type */
	for (i = 0; i < coln; i++) {
		auto n = var_to_str(t->col_names[i]);
		auto Y = T::TColTy_short[i];
		auto s = std::format("{}:{}", n, Y);
		head[i] = s;
		Ls[i] = s.size();
	}

	/* format each row */
	for (y = 0, rown = 0; y < t->row_cap; y++) {
		if (t->init[y]) {
			rown++;
			row = new std::string[coln];
			for (x = 0; x < coln; x++) {
				std::stringstream ss;
				switch (t->col_tys[x]) {
				#define col_xy(Y, x, y) ss << (((Y*)t->cols[x])[y])
				CASE(T::TInt, col_xy(i32, x, y))
				CASE(T::TDbl, col_xy(f64, x, y))
				CASE(T::TChr, col_xy(Chr, x, y))
				default:
					ss << '{' << t->col_type(x);
					ss << "???}";
				}
				auto col = ss.str();
				Ls[x] = std::max(Ls[x], col.size());
				row[x] = col;
			}

			cells.push_back(row);
		}
	}

	/*
	 * make the final string
	 */
	std::stringstream ss;

	/* a line */
	for (S i = 0; i < U::sum_vec<S>(Ls, coln) + coln + 1; i++) ss << '=';
	ss << std::endl;

	/* header */
	ss << '|';
	for (S i = 0; i < coln; i++) ss << head[i] << '|';
	ss << std::endl;

	/* a line */
	for (S i = 0; i < U::sum_vec<S>(Ls, coln) + coln + 1; i++) ss << '=';
	ss << std::endl;

	/* cells */
	y = 0;
	for (auto &r : cells) {
		ss << '|';
		for (x = 0; x < coln; x++) {
			auto c = r[x];
			S L = Ls[x] - c.size();
			ss << c;
			for (i = 0; i < L; i++) ss << ' ';
			ss << '|';
		}
		ss << std::endl;
		y++;
	}

	delete[] head;
	return ss.str();
}
