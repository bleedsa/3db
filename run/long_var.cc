#include <Asm.h>
#include <db.h>

int main() {
	auto long_name = "12345678901234567890";
	Three::init();

	/* make a dummy asm */
	auto a = Asm::Asm();
	a.push_bod(VM::Bod(0, 0));
	a.push_in(Bc::In(123.456));
	a.push_in(Bc::In(Bc::STORE, long_name));
	a.push_in(Bc::In(Bc::LOAD, long_name));
	a.push_in(Bc::In());

	/* display the name */
	auto s = var_to_str(str_to_var(long_name));
	std::cout << s << std::endl;
	delete[] s;

	/* exe */
	auto r = a.exe();
	if (!r) {
		std::cerr << r.error() << std::endl;
		return -1;
	}

	/* fmt */
	auto q = *r;
	std::cout << Fmt::Fmt(&q) << std::endl;

	/* grab the entry */
	auto ent = Db::get(long_name);
	std::cout << Fmt::Fmt(*ent) << std::endl;

	Three::deinit();
	return 0;
}
