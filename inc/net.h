#ifndef __3DB_NET_H__
#define __3DB_NET_H__

#include <tuple>

#include <u.h>

namespace Net {
	R<std::tuple<std::string, std::string>> parse_addr_port(char *x);
	int connect_host(const char *addr_port);

	char *send_str(int sock, std::string *str);
	char *recv_str(int sock, std::string *str);
}

#endif
