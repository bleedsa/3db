#ifndef __3DB_NET_ASM_H__
#define __3DB_NET_ASM_H__

#include <Asm.h>

namespace Net {
	char *send_Asm(int sock, Asm::Asm *x);
	char *recv_Asm(int sock, Asm::Asm *x);
}

#endif
