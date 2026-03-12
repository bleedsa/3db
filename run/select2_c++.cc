#include <db.h>
#include <cmd.h>

typedef double xmm_t __attribute__((vector_size(16)));

int main() {
	Three::init();

	/* craft a table */
	auto t = T::T(
		A::A{"ints", "dbl vecs", "strs"},
		A::A{T::TInt, T::TDBL, T::TCHR}
	);
	xmm_t dbl = {1.0, 2.0};
	var_t str = str_to_var("0123456789abcdef");
	t.insert(001, 1, &dbl, 2, &str, 16); dbl *= 0.66; str += 4;
	t.insert(002, 2, &dbl, 2, &str, 16); dbl *= 0.66; str += 4;
	t.insert(003, 3, &dbl, 2, &str, 16);

	/* add it to the database and print */
	auto ent = Db::add("table1", t);
	std::cout << Fmt::Fmt(ent) << std::endl;

	try {
		/* make a select command */
		auto cmd = Cmd::Cmd(Cmd::SELECT, 0)
			.entry("table1")
			.columns(A::A{"ints", "strs"});

		auto res = cmd.exe();
		if (!res) throw res.error();
		auto x = *res;
		auto y = **x;

		std::cout << Fmt::Fmt(&y) << std::endl;
	} catch (std::string e) {
		std::cerr << e << std::endl;
	}

	Three::deinit();
	return 0;
}
