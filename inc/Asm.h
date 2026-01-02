#ifndef __3DB_ASM_H__
#define __3DB_ASM_H__

#include <u.h>
#include <bc.h>
#include <vm.h>

namespace Asm {
	struct Asm {
		u32 inL, in_cap, bodL, bod_cap;
		Bc::In *ins;
		VM::Bod *bods;

		Asm();
		~Asm();

		Asm(const Asm &x);
		const Asm &operator=(const Asm &x);

		void reZ();
		void push_in(Bc::In x);
		void push_bod(VM::Bod x);
	};
}

#endif
