#include <db.h>

typedef double xmm_t __attribute__((vector_size(16)));

int main() {
	/* craft a table */
	auto t = T::T(
		A::A{"ints", "dbl vecs"},
		A::A{T::TInt, T::TDBL}
	);
	xmm_t dbl = {1.0, 2.0};
	t.insert(001, 1, &dbl, 2); dbl *= 0.01;
	t.insert(002, 2, &dbl, 2); dbl *= 0.01;
	t.insert(003, 3, &dbl, 2);

	/* add it to the database and print */
	auto ent = Db::add("table1", t);
	std::cout << Fmt::Fmt(ent) << std::endl;

	return 0;
}
