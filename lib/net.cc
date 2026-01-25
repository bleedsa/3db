#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <sstream>
#include <cstring>
#include <tuple>
#include <expected>

#include <str.h>
#include <net.h>
#include <mem.h>

auto Net::connect_host(const char *addr_port) -> int {
	int ret, sock;
	const char *addr, *port;
	struct addrinfo hints, *res;

	/* split up the addr port string */
	auto r = parse_addr_port((char*)addr_port);
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

	freeaddrinfo(res);

	return sock;
}

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

static constexpr S headZ = Z(S);

auto Net::send_str(int sock, std::string *str) -> char* {
	int sent;
	u8 head[headZ], *body;
	char *c;
	S L, z;

	/* set the length */
	L = str->size();
	*(S*)head = L;

	/* send the header */
	dbg(std::cout << "sending str header " << L << " long...");
	if ((sent = send(sock, (const void*)head, headZ, 0)) < 0) {
		return A_err("failed to send str header: {}", strerror(errno));
	}
	dbg(std::cout << "ok " << sent << std::endl);

	/* make the body */
	z = Z(char) * L;
	body = mk<u8>(z);
	c = (char*)str->c_str();
	memcpy(body, c, z);

	/* send the body */
	dbg(std::cout << "sending str body...");
	if ((sent = send(sock, (const void*)body, z, 0)) == -1) {
		return A_err("failed to send str body: {}", strerror(errno));
	}
	dbg(std::cout << "ok " << sent << std::endl);

	free(body);
	return nullptr;
}

auto Net::recv_str(int sock, std::string *str) -> char* {
	int got;
	u8 head[headZ], *body;
	S L, z;

	/* get the header */
	dbg(std::cout << "recv'ing str header...");
	if ((got = recv(sock, head, headZ, 0)) < 0) {
		return A_err("failed to recv str header: {}", strerror(errno));
	}
	dbg(std::cout << "ok " << got << std::endl);

	/* deconstruct the header */
	L = *(S*)head;

	/* make the body buffer */
	z = Z(char) * L;
	body = mk<u8>(z);

	/* recv the body */
	dbg(std::cout << "recv'ing str body " << L << " long...");
	if ((got = recv(sock, body, z, 0)) < 0) {
		return A_err("failed to recv str body: {}", strerror(errno));
	}
	dbg(std::cout << "ok " << got << std::endl);

	/* set the return str */
	str->~basic_string();
	*str = std::string();

	/* copy the string contents */
	for (S i = 0; i < L; i++) str->push_back(body[i]);

	return nullptr;
}
