#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>

#include <u.h>
#include <str.h>
#include <db.h>
#include <net.h>
#include <Asm.h>
#include <net/Asm.h>
#include <net/Q.h>

struct addrinfo hints, *res;
int sock;

inl auto connect_host(char *addr_port) -> void {
	int ret;
	const char *addr, *port;
	auto r = Net::parse_addr_port(addr_port);
	if (!r) fatal("failed to parse address: {}", r.error());
	auto [addr_str, port_str] = *r;

	addr = addr_str.c_str();
	port = port_str.c_str();

	/* get address info */
	memset(&hints, 0, Z(hints));
	hints.ai_family = AF_UNSPEC; /* v4 or v6 */
	hints.ai_socktype = SOCK_STREAM;
	if ((ret = getaddrinfo(addr, port, &hints, &res)) != 0) {
		fatal("getaddrinfo(): {}", gai_strerror(ret));
	}

	/* make socket */
	sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if (sock == -1) fatal("socket(): {}", strerror(errno));

	/* make connection */
	ret = connect(sock, res->ai_addr, res->ai_addrlen);
	if (ret < 0) fatal("connect(): {}", strerror(errno));
}

int main(int argc, char **argv) {
	Q::Q q;
	if (argc < 2) fatal("usage: {} [addr]", argv[0]);

	connect_host(argv[1]);
	Three::init();

	auto a = Asm::Asm();
	auto v = str_to_var("float1");
	a.push_bod(VM::Bod(0, 0));
	a.push_in(Bc::In(1234.3));
	a.push_in(Bc::In(Bc::STORE, v));
	a.push_in(Bc::In(Bc::LOAD, v));
	a.push_in(Bc::In());

	auto err = Net::send_Asm(sock, &a);
	if (err) {
		std::cerr << err << std::endl;
		goto end;
	}

	// err = Net::recv_Q(sock, &q);
	if (err) {
		std::cerr << err << std::endl;
		goto end;
	}

end:
	close(sock);
	Three::deinit();
}
