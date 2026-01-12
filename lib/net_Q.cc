#include <sys/socket.h>

#include <str.h>
#include <net/Q.h>

auto Net::send_Q(int sock, Q::Q *x) -> char* {
	int sent;
	u8 buf[Z(Q::Q)];

	dbg(std::cout << "sending Q " << Fmt::Fmt(x) << "...");
	if ((sent = send(sock, (const void*)buf, Z(Q::Q), 0)) == -1) {
		return A_err("failed to send Q: {}", strerror(errno));
	}
	dbg(std::cout << "ok " << sent << std::endl);

	return nullptr;
}

/*
auto Net::recv_Q(int sock, Q::Q *x) -> char* {
	dbg(std::cout << "recv()...");
	if ((got = recv(sock, (void*)head, Z(u32)*3, 0)) == -1) {
		return A_err("failed to recv header: {}", strerror(errno));
	}
	dbg(std::cout << "ok " << got << std::endl);

*/
