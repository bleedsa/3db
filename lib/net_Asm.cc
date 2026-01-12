#include <sstream>
#include <sys/socket.h>

#include <mem.h>
#include <net.h>
#include <net/Asm.h>

auto Net::send_Asm(int sock, Asm::Asm *x) -> char* {
	S headZ, bodyZ;
	u8 head[Z(u32)*3], *body;
	u32 *u32s;
	Bc::In *ins;
	VM::Bod *bods;
	int sent;

	/* calc the buffer size */
	headZ = Z(u32)*3; /* header values */
	bodyZ = (Z(Bc::In)*x->inL) + (Z(VM::Bod)*x->bodL);
	/* alloc */
	body = mk<u8>(bodyZ);

	/* copy header */
	u32s = (u32*)head;
	u32s[0] = x->start, u32s[1] = x->inL, u32s[2] = x->bodL;

	/* send the header */
	std::cout << "sending header...";
	sent = send(sock, (const void*)head, headZ, 0);
	if (sent == -1) {
		return A_err("failed to send header: {}", strerror(errno));
	}
	std::cout << "ok " << sent << std::endl;

	/* copy the instructions */
	ins = (Bc::In*)body;
	memcpy(ins, x->ins, Z(Bc::In)*x->inL);

	/* copy the bodies */
	bods = (VM::Bod*)(ins+x->inL);
	memcpy(bods, x->bods, Z(VM::Bod)*x->bodL);

	/* send the body */
	std::cout << "sending body " << '(' << bodyZ << ")...";
	sent = send(sock, (const void*)ins, bodyZ, 0);
	if (sent == -1) {
		return A_err("failed to send body: {}", strerror(errno));
	}
	std::cout << "ok " << sent << std::endl;

	free(body);

	return nullptr;
}

auto Net::recv_Asm(int sock, Asm::Asm *x) -> char* {
	x->~Asm();

	u32 head[3];
	int got;
	S bytes;
	u8 *body;

	/* recv the head */
	dbg(std::cout << "recv()...");
	if ((got = recv(sock, (void*)head, Z(u32)*3, 0)) == -1) {
		return A_err("failed to recv header: {}", strerror(errno));
	}
	dbg(std::cout << "ok " << got << std::endl);

	/* copy the values */
	x->start = head[0];
	x->inL = head[1],    x->bodL = head[2];
	x->in_cap = head[1], x->bod_cap = head[2];

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
	x->ins = mk<Bc::In>(x->inL);
	memcpy(x->ins, body, Z(Bc::In)*x->inL);

	/* copy the bodies */
	x->bods = mk<VM::Bod>(x->bodL);
	memcpy(x->bods, (void*)(((Bc::In*)body)+x->inL), Z(VM::Bod)*x->bodL);

	free(body);

	return nullptr;
}
