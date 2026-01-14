#include <vector>
#include <sstream>

#include <T.h>

template<typename Y>
inl auto fmt_vector(std::stringstream &ss, Y *ptr, S len) -> void {
	auto a = A::A<Y>(ptr, len);
	ss << Fmt::Fmt(&a);
}

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
		delete[] n;
	}

	/* format each row */
	for (y = 0, rown = 0; y < t->row_cap; y++) {
		if (t->init[y]) {
			rown++;
			row = new std::string[coln];
			for (x = 0; x < coln; x++) {
				std::stringstream ss;
				switch (t->col_tys[x]) {
				#define fmt_col(Y) ss << ((Y*)t->cols[x])[y]
				CASE(T::TInt, fmt_col(i32))
				CASE(T::TDbl, fmt_col(f64))
				CASE(T::TChr, fmt_col(Chr))
				#define fmt_vec(Y) { \
					auto ptr = ((S**)t->cols[x])[y]; \
					auto buf = (Y*)(ptr+1); \
					auto len = *ptr; \
					fmt_vector<Y>(ss, buf, len); \
				}
				CASE(T::TINT, fmt_vec(i32))
				CASE(T::TDBL, fmt_vec(f64))
				CASE(T::TCHR, fmt_vec(Chr))
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
	auto L_sum = U::sum_vec<S>(Ls, coln);

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
	for (i = 0; i < coln; i++) {
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
		for (x = 0; x < coln; x++) {
			auto c = r[x];
			ss << c;
			pad(Ls[x] - c.size(), ' ');
			ss << '|';
		}
		ss << std::endl;
		y++;
	}

	delete[] head;
	return ss.str();
}
