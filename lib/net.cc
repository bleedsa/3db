#include <sstream>
#include <cstring>
#include <tuple>
#include <expected>

#include <net.h>

auto Net::parse_addr_port(char *x) -> R<std::tuple<std::string, std::string>> {
	std::stringstream addr, port;
	char *ptr;
	S col /* colon idx */, i, L;

	ptr = strchr(x, ':');
	if (ptr == nullptr) return std::unexpected("malformed address: no ':'");
	col = (S)(ptr - x);
	L = strlen(x);

	for (i = 0; i < col; i++) addr << x[i];
	for (i = col+1; i < L; i++) port << x[i];

	auto tup = std::tuple<std::string, std::string>{addr.str(), port.str()};
	return tup;
}


