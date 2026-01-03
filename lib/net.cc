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
	Bc::In *ins;
	VM::Bod *bods;
	int sent;

	/* calc the buffer size */
	headZ = Z(u32)*3; /* header values */
	bodyZ = (Z(Bc::In)*x->inL)+ (Z(VM::Bod)*x->bodL);
	/* alloc */
	buf = (u8*)malloc(headZ + bodyZ);

	/* copy header */
	u32s = (u32*)buf;
	u32s[0] = x->start, u32s[1] = x->inL, u32s[2] = x->bodL;

	/* send the header */
	std::cout << "sending header...";
	sent = send(sock, (const void*)buf, headZ, 0);
	if (sent == -1) {
		return A_err("failed to send header: {}", strerror(errno));
	}
	std::cout << "ok " << sent << std::endl;

	/* copy the instructions */
	ins = (Bc::In*)(buf+headZ);
	for (S i = 0; i < x->inL; i++) ins[i] = x->ins[i];

	/* copy the bodies */
	bods = (VM::Bod*)(ins+x->inL);
	for (S i = 0; i < x->bodL; i++) bods[i] = x->bods[i];

	/* send the body */
	std::cout << "sending body " << '(' << bodyZ << ")...";
	sent = send(sock, (const void*)ins, bodyZ, 0);
	if (sent == -1) {
		return A_err("failed to send body: {}", strerror(errno));
	}
	std::cout << "ok " << sent << std::endl;

	return nullptr;
}

auto Net::recv_Asm(int sock, Asm::Asm *x) -> char* {
	x->~Asm();

	int got;
	u32 *head = mk<u32>(2);
	S bytes;
	u8 *body;

	/* recv the head */
	dbg(std::cout << "recv()...");
	if ((got = recv(sock, (void*)head, Z(u32)*3, 0)) == -1) {
		return A_err("failed to recv header: {}", strerror(errno));
	}
	dbg(std::cout << "ok " << got << std::endl);

	/* copy the values */
	dbg(std::cout << "copy header values...");
	x->start = head[0];
	x->inL = head[1],    x->bodL = head[2];
	x->in_cap = head[1], x->bod_cap = head[2];
	dbg(std::cout << "ok" << std::endl);

	/* alloc the body */
	bytes = (Z(Bc::In)*x->inL) + (Z(VM::Bod)*x->bodL);
	body = mk<u8>(bytes);

	/* recv the body */
	dbg(std::cout << "recv()...");
	if ((got = recv(sock, (void*)body, bytes, 0)) == -1) {
		return A_err("failed to recv body: {}", strerror(errno));
	}
	dbg(std::cout << "ok " << got << std::endl);

	/* copy the instructions */
	dbg(std::cout << "copy instructions...");
	x->ins = mk<Bc::In>(x->inL);
	memcpy(x->ins, body, Z(Bc::In)*x->inL);
	dbg(std::cout << "ok" << std::endl);

	/* copy the bodies */
	dbg(std::cout << "copy bodies...");
	x->bods = mk<VM::Bod>(x->bodL);
	memcpy(x->bods, (void*)(((Bc::In*)body)+x->inL), Z(VM::Bod)*x->bodL);
	dbg(std::cout << "ok" << std::endl);

	return nullptr;
}
