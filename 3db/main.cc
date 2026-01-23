#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <cerrno>
#include <mutex>
#include <unistd.h>
#include <csignal>

#include <u.h>
#include <str.h>
#include <db.h>
#include <fs.h>
#include <net.h>
#include <Asm.h>
#include <net/Asm.h>
#include <net/Q.h>

#include "cfg.h"

#ifndef NAT
__static_yoink("__die");
#endif

struct addrinfo hints, *res;
int sock;
std::mutex sock_mut;
bool main_exit = false;

auto sigint_handler(int) -> void {
	close(sock);
	Three::deinit();
	main_exit = true;
}

inl auto bind_host(const char *port) -> void {
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
	struct sigaction sa;

	sa.sa_handler = sigint_handler;
	sa.sa_flags = 0;
	sigemptyset(&sa.sa_mask);
	if (sigaction(SIGINT, &sa, NULL) < 0) perror("sigaction");

	path = argc<2 ? (char*)"/zip/cfg.ini" : argv[1];

	/* check if the config exists */
	if (!Fs::F_exists(path)) {
		std::cerr << "cfg file does not exist: " << path << std::endl;
		return -1;
	}

	/* load the cfg */
	auto cfg = Cfg(path);
	/* load the db file */
	auto db = cfg.file.c_str();

	if (Fs::F_exists(db)) Db::load(db);

	/* bind & init 3 */
	bind_host(cfg.port.c_str());
	Three::init();

	/* start listening */
	ret = listen(sock, 10);
	if (ret == -1) fatal("listen(): {}", strerror(errno));

	while (!main_exit) {
		auto str = std::string();
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

		if ((err = Net::recv_str(fd, &str))) {
			std::cerr << err << std::endl;
			goto end;
		}

//		/* write the data to disk */
//		Db::write(db);
//
//		/* send the result back */
//		err = Net::send_Q(fd, &q);
//		if (err) {
//			std::cerr << err << std::endl;
//			goto end;
//		}

end:
		close(fd);
	}

	close(sock);
	Three::deinit();
}
