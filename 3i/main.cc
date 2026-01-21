
#include <u.h>
#include <str.h>
#include <db.h>
#include <net.h>
#include <Asm.h>
#include <net/Asm.h>
#include <net/Q.h>
#include <cli.h>

template<typename X>
inl auto un(R<X> r) -> X {
	if (r) return *r;
	else {
		std::cout << r.error() << std::endl;
		exit(-1);
	}
}

int main(int argc, char **argv) {
	Q::Q q(1);
	if (argc < 2) fatal("usage: {} [addr]", argv[0]);

	Three::init();

	auto cli = Cli::Cli(argv[1]);
	
	auto nil = un(cli.set("an atomic int", &q));
	std::cout << Fmt::Fmt(&nil) << std::endl;

	q = Q::Q(A::A{1, 2, 3, 4, 5, 6, 7, 8, 9, 0});
	nil = un(cli.set("vec1", &q));
	std::cout << Fmt::Fmt(&nil) << std::endl;

	Three::deinit();
}
