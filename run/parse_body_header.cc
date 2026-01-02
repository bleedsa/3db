#include <iostream>

#include <u.h>
#include <AsmParser.h>

std::string src =
	"1234,5678";

int main() {
	Three::init();

	auto a = Asm::AsmParser(src);
	auto r = a.body_header();
	if (!r) {
		std::cout << r.error() << std::endl;
		exit(-1);
	}
	auto b = *r;
	std::cout << b.vars << ' ' << b.start << std::endl;

	Three::deinit();
}
