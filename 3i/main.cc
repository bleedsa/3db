
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
	char *err;
	int sock;
	std::string str;
	if (argc < 2) fatal("usage: {} [addr]", argv[0]);

	Three::init();

	sock = Net::connect_host((const char*)argv[1]);
	str = "!10";

	if ((err = Net::send_str(sock, &str)))
		fatal("{}", err);

	Three::deinit();
}
