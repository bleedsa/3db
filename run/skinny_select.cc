#include <db.h>
#include <cmd.h>

typedef double xmm_t __attribute__((vector_size(16)));
typedef i32 xmm2_t __attribute__((vector_size(16)));

int main() {
	Three::init();

	/* craft a table */
	auto t = T::T(
		A::A{
			"ints", "dbl vecs", "strs", "dbls",
			"ints2", "ints3", "ints4", "ints5",
			"dbls2", "dbls3", "dbls4", "dbls5",
			"INTS", "INTS2", "INTS3", "INTS4",
		},
		A::A{
			T::TInt, T::TDBL, T::TCHR, T::TDbl,
			T::TInt, T::TInt, T::TInt, T::TInt,
			T::TDbl, T::TDbl, T::TDbl, T::TDbl,
			T::TINT, T::TINT, T::TINT, T::TINT,
		}
	);
	xmm_t dbl = {1.0, 2.0};
	xmm2_t ints = {1, 2, 3, 4};
	var_t str = str_to_var("0123456789abcdef");

	/* perform a bunch of insertions */
	for (S i = 0; i < 300000; i++) {
		t.insert(i,
			1, &dbl, 2, &str, 15, 1.23,
			1, 2, 3, 4,
			1.23, 4.56, 7.89, 0.12,
			&ints, 1, &ints, 2, &ints, 3, &ints, 4
		);
		dbl *= 0.66;
		ints *= 2;
	}

	/* add it to the database and print */
	Db::add("table1", t);

	try {
		/* make a select command */
		auto cmd = Cmd::Cmd(Cmd::SELECT, 0)
			.entry("table1")
			.columns(A::A{"INTS3"});

		auto res = cmd.exe();
		if (!res) throw res.error();
		auto x = *res;
		auto y = **x;
	} catch (std::string e) {
		std::cerr << e << std::endl;
	}

	Three::deinit();
	return 0;
}
