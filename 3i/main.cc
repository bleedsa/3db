
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

	auto a = Q::Q(A::A{1, 2, 3, 4, 5, 6, 7, 9});
	q = un(cli.set("vec1", &a));
	std::cout << Fmt::Fmt(&q) << std::endl;

	q = un(cli.get("vec1"));
	std::cout << Fmt::Fmt(&q) << std::endl;

	Three::deinit();
}
