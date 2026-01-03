#ifndef __3DB_ASM_H__
#define __3DB_ASM_H__

#include <u.h>
#include <bc.h>
#include <vm.h>

namespace Asm {
	/*
	 * A structure containing bytecode asm. This has a flat repr that can be sent
	 * over the network (see inc/net/Asm.h). 
	 * NOTE: if you update any values within this struct, it will break network
	 * code.
	 */
	struct Asm {
		u32 inL, in_cap, /* instr bounds */ bodL, bod_cap /* body bounds */;
		/* buffer of instructions */
		Bc::In *ins;
		/* buffer of bodies */
		VM::Bod *bods;

		Asm();    /* empty with cap 16 */
		Asm(u8*); /* construct out of a raw buffer */
		~Asm();

		Asm(const Asm &x);
		const Asm &operator=(const Asm &x);

		/* modify the internal buffers */
		void reZ();
		void push_in(Bc::In x);
		void push_bod(VM::Bod x);
	};
}

#endif
