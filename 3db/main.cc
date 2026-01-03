#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <cerrno>
#include <mutex>

#include <u.h>
#include <str.h>
#include <db.h>
#include <net.h>
#include <Asm.h>
#include <net/Asm.h>

struct addrinfo hints, *res;
int sock;
std::mutex sock_mut;

inl auto bind_host(char *port) -> void {
	int ret;

	/* get address info */
	memset(&hints, 0, Z(hints));
	hints.ai_family = AF_UNSPEC; /* v4 or v6 */
	hints.ai_socktype = SOCK_STREAM;
	if ((ret = getaddrinfo("0.0.0.0", port, &hints, &res)) != 0) {
		fatal("getaddrinfo(): {}", gai_strerror(ret));
	}

	/* make socket */
	sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if (sock == -1) fatal("socket(): {}", strerror(errno));

	/* bind to port */
	ret = bind(sock, res->ai_addr, res->ai_addrlen);
	if (ret == -1) fatal("bind(): {}", strerror(errno));
}

int main(int argc, char **argv) {
	int ret, fd;
	struct sockaddr_storage them;
	socklen_t addrZ = Z(them);
	char *err;

	if (argc < 2) fatal("usage: {} [port]", argv[0]);

	bind_host(argv[1]);
	Three::init();

	ret = listen(sock, 10);
	if (ret == -1) fatal("listen(): {}", strerror(errno));

	while (true) {
		auto x = Asm::Asm();
		auto G = LOCK(sock_mut);

		fd = accept(sock, (struct sockaddr*)&them, &addrZ);
		if (fd == -1) {
			std::cerr << strerror(errno) << std::endl;
			continue;
		}	

		err = Net::recv_Asm(fd, &x);
		if (err) {
			std::cerr << err << std::endl;
			continue;
		}

		std::cout << "instructions: " << x.inL << '(' << x.in_cap << ')'
		       	<< std::endl;
		std::cout << "bodies: " << x.bodL << '(' << x.bod_cap << ')'
			<< std::endl;

		for (S i = 0; i < x.inL; i++) {
			auto in = &x.ins[i];
			std::cout << (S)in->ty << ' ';
		}
		std::cout << std::endl;

		for (S i = 0; i < x.bodL; i++) {
			auto b = &x.bods[i];
			std::cout << b->vars << ' ' << b->start << std::endl;
		}

		x.exe_body(x.start);
		
		close(fd);
	}

	close(sock);
	Three::deinit();
}
