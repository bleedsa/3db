
#include <u.h>
#include <str.h>
#include <db.h>
#include <net.h>
#include <Asm.h>
#include <net/Asm.h>
#include <net/Q.h>
#include <cmd.h>

template<typename X>
inl auto un(R<X> r) -> X {
	if (r) return *r;
	else {
		std::cout << r.error() << std::endl;
		exit(-1);
	}
}

int main(int argc, char **argv) {
	if (argc < 2) fatal("usage: {} [addr]", argv[0]);

	Three::init();

	Cmd::Create cmd = Cmd::Create(argv[1])
		.entry("table0");

	std::cout << Fmt::Fmt(&cmd) << std::endl;

	auto ent = un(cmd.Cmd::send());
	std::cout << Fmt::Fmt(&ent) << std::endl;

	Three::deinit();
}
