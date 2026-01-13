#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <cerrno>
#include <mutex>
#include <unistd.h>

#include <u.h>
#include <str.h>
#include <db.h>
#include <fs.h>
#include <net.h>
#include <Asm.h>
#include <net/Asm.h>
#include <net/Q.h>

#ifndef NAT
__static_yoink("__die");
#endif

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
	char *err, *path;
	Q::Q q;
	R<Q::Q> res;

	if (argc < 3) fatal("usage: {} [file] [port]", argv[0]);
	path = argv[1];

	if (Fs::F_exists(path)) Db::load(path);

	bind_host(argv[2]);
	Three::init();

	ret = listen(sock, 10);
	if (ret == -1) fatal("listen(): {}", strerror(errno));

	for (;;) {
		auto x = Asm::Asm();
		auto G = LOCK(sock_mut);

		/* get the connection */
		std::cout << "waiting for mail...";
		fflush(stdout);
		fd = accept(sock, (struct sockaddr*)&them, &addrZ);
		if (fd == -1) {
			std::cerr << strerror(errno) << std::endl;
			goto end;
		}
		std::cout << "ok " << fd << std::endl;

		/* read the assembly */
		err = Net::recv_Asm(fd, &x);
		if (err) {
			std::cerr << err << std::endl;
			goto end;
		}

		for (S i = 0; i < x.inL; i++) {
			auto in = &x.ins[i];
			std::cout << (S)in->ty << ' ';
		}
		std::cout << std::endl;

		for (S i = 0; i < x.bodL; i++) {
			auto b = &x.bods[i];
			std::cout << b->vars << ' ' << b->start << std::endl;
		}

		/* execute the assembly */
		res = x.exe();
		if (!res) {
			std::cerr << res.error() << std::endl;
			goto end;
		}

		q = *res;
		std::cout << Fmt::Fmt(&q) << std::endl;

		/* write the data to disk */
		Db::write(path);

		/* send the result back */
		err = Net::send_Q(fd, &q);
		if (err) {
			std::cerr << err << std::endl;
			goto end;
		}

end:
		close(fd);
	}

	close(sock);
	Three::deinit();
}
