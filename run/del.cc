#include <db.h>
#include <cmd.h>

int main() {
	Three::init();

	try {
		Cmd::Cmd cmds[] = {
			Cmd::Cmd(Cmd::CREATE, 0)
				.entry("0")
				.type(Db::Tab)
				.columns({
					std::tuple{"int", T::TInt},
					std::tuple{"dbl", T::TDbl}
				}),
			Cmd::Cmd(Cmd::INSERT, 0)
				.entry("0")
				.row(1)
				.columns({Q::Q(1), Q::Q(2.345)}),
			Cmd::Cmd(Cmd::INSERT, 0)
				.entry("0")
				.row(2)
				.columns({Q::Q(6), Q::Q(7.890)}),
			Cmd::Cmd(Cmd::INSERT, 0)
				.entry("0")
				.row(3)
				.columns({Q::Q(10), Q::Q(10.5)}),
			Cmd::Cmd(Cmd::DEL, 0)
				.entry("0")
				.where(
					Cmd::Where(Cmd::Gt)
						.name("int")
						.value(1)
				),
		};

		for (auto &cmd : cmds) {
			auto res = cmd.exe();
			if (!res) throw res.error();
			auto x = *res;
			auto y = *x;
			std::cout << Fmt::Fmt(y) << std::endl;
		}
	} catch (std::string &e) {
		std::cerr << e << std::endl;
	}

	Three::deinit();
}
