#include <sys/socket.h>

#include <db.h>
#include <net/Ent.h>

auto Net::send_Ent(int sock, Db::Ent *x) -> char* {
	int sent;

	auto [B, L] = x->to_bytes(); 

	/* send header */
	dbg(std::cout<<"sending entry header of "<<L<<'('<<Z(u64)<<")...");
	if ((sent = send(sock, (const void*)&L, Z(u64), 0)) == -1) {
		return A_err("failed to send Q header: {}", strerror(errno));
	}
	dbg(std::cout<<"ok "<<sent<<std::endl);

	/* send the body */
	dbg(std::cout<<"sending entry body of "<<L<<" bytes...");
	if ((sent = send(sock, (const void*)B, L, 0)) == -1) {
		return A_err("failed to send entry body: {}", strerror(errno));
	}
	dbg(std::cout<<"ok "<<sent<<std::endl);

	delete[] B;
	return nullptr;
}

auto Net::recv_Ent(int sock, Db::Ent *x) -> char* {
	int got;
	u64 L, z;
	u8 *body;

	/* recv the length */
	dbg(std::cout << "recv'ing entry header...");
	if ((got = recv(sock, (void*)&z, Z(u64), 0)) == -1) {
		return A_err("recv entry header: {}", strerror(errno));
	} else if (got == 0) return A_err("connection reset: {}", strerror(errno));
	dbg(std::cout << "ok "<<got<<std::endl);

	/* recv body */
	body = new u8[z];
	dbg(std::cout << "recv'ing entry body...");
	if ((got = recv(sock, (void*)body, z, 0)) == -1) {
		delete[] body;
		return A_err("failed to recv entry body: {}", strerror(errno));
	}
	dbg(std::cout << "ok " << got << std::endl);

	/* decode */
	*x = Db::Ent(body);

	delete[] body;
	return nullptr;
}
