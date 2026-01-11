#include <sys/socket.h>

#include <str.h>
#include <net/Q.h>

auto Net::send_Q(int sock, Q::Q *x) -> char* {
	int sent;
	u8 buf[Z(Q::Q)];

	dbg(std::cout << "sending Q " << Fmt::Q(x) << "...");
	if ((sent = send(sock, (const void*)buf, Z(Q::Q), 0)) == -1) {
		return A_err("failed to send Q: {}", strerror(errno));
	}
	dbg(std::cout << "ok " << sent << std::endl);

	return nullptr;
}


	/* send the header */
/*
	std::cout << "sending header...";
	sent = send(sock, (const void*)head, headZ, 0);
	if (sent == -1) {
		return A_err("failed to send header: {}", strerror(errno));
	}
	std::cout << "ok " << sent << std::endl;
*/
