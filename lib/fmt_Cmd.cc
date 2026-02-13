#include <sstream>

#include <cmd.h>

auto Fmt::Fmt(Cmd::Cmd *x) -> std::string {
	std::stringstream ss;

	ss << "(Cmd::Cmd){" << x->type() << '}';

	return ss.str();
}
