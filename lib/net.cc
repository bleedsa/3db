#include <sstream>
#include <sys/socket.h>

#include <mem.h>
#include <net.h>
#include <net/Asm.h>

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

auto Net::send_Asm(int sock, Asm::Asm *x) -> char* {
	S headZ, bodyZ;
	u8 *buf;
	u32 *u32s;
	int sent;

	/* calc the buffer size */
	headZ = Z(u32)*2 /* header values */;
	bodyZ = (Z(Bc::In)*x->inL)+ (Z(VM::Bod)*x->bodL);
	/* alloc */
	buf = (u8*)malloc(headZ + bodyZ);

	/* copy header */
	u32s = (u32*)buf;
	u32s[0] = x->inL, u32s[1] = x->bodL;

	/* send the header */
	std::cout << "sending header...";
	sent = send(sock, (const void*)buf, headZ, 0);
	if (sent == -1) {
		return A_err("failed to send header: {}", strerror(errno));
	}
	std::cout << "ok " << sent << std::endl;

	return nullptr;
}

auto Net::recv_Asm(int sock, Asm::Asm *x) -> char* {
	int got;
	u32 *head = mk<u32>(2);

	if ((got = recv(sock, (void*)head, Z(u32)*2, 0)) == -1) {
		return A_err("failed to recv header: {}", strerror(errno));
	}

	x->inL = head[0],    x->bodL = head[1];
	x->in_cap = head[0], x->bod_cap = head[1];

	return nullptr;
}
