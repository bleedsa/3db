#include <sstream>

#include <sse.h>

auto Fmt::Fmt(xmm64_t *x) -> std::string {
	std::stringstream ss;

	ss << '(' << (*x)[0] << ';' << (*x)[1] << ')';

	return ss.str();
}
