
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

	try {
		Cmd::Cmd cmds[] = {
			Cmd::Cmd(Cmd::DEL, argv[1])
				.entry("debts")
				.where(
					Cmd::Where(Cmd::Gt)
						.name("amount")
						.value(30.0)
				)
		};

		for (auto &cmd : cmds) {
			auto res = cmd.send();
			if (!res) std::cout << res.error() << std::endl;
			else {
				auto e = *res;
				std::cout << Fmt::Fmt(&e) << std::endl;
			}
		}
	} catch (std::string &e) {
		std::cerr << "'exc: " << e << std::endl;
	} catch (const char *e) {
		std::cerr << "'exc: " << e << std::endl;
	}

	Three::deinit();
}
