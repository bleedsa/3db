#ifndef __3DB_NET_H__
#define __3DB_NET_H__

#include <tuple>

#include <u.h>
#include <cmd.h>
#include <net/Cmd.h>

namespace Net {
	R<std::tuple<std::string, std::string>> parse_addr_port(char *x);
	int connect_host(const char *addr_port);

	char *send_str(int sock, std::string *str);
	char *recv_str(int sock, std::string *str);

	/* a TCP stream abstraction */
	struct TcpS {
		int sock;

		inl TcpS(int sock) : sock{sock} {}
		inl ~TcpS() {}

		inl auto cpy(const TcpS &x) -> void {sock = x.sock;}
		inl TcpS(const TcpS &x) {cpy(x);}
		inl auto operator=(const TcpS &x) -> TcpS& {cpy(x);return*this;}
	};

}

const Net::TcpS &operator>>(const Net::TcpS &x, u8 &y);
const Net::TcpS &operator<<(const Net::TcpS &x, u8 &y);

const Net::TcpS &operator<<(const Net::TcpS &x, u64 &y);
const Net::TcpS &operator>>(const Net::TcpS &x, u64 &y);

const Net::TcpS &operator<<(const Net::TcpS &x, var_t &y);
const Net::TcpS &operator>>(const Net::TcpS &x, var_t &y);

const Net::TcpS &operator<<(const Net::TcpS &x, std::tuple<u8*, u64> &y);
const Net::TcpS &operator>>(const Net::TcpS &x, std::tuple<u8*, u64> &y);

const Net::TcpS &operator>>(const Net::TcpS &x, Cmd::Insert &y);
const Net::TcpS &operator>>(const Net::TcpS &x, Cmd::Create &y);

#endif
