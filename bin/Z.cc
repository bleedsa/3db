#include <iostream>

#include <Q.h>
#include <db.h>
#include <bc.h>
#include <vm.h>
#include <Asm.h>
#include <T.h>

struct Z_t {
	const char *name;
	S size;

	Z_t(const char *name, S size) : name{name}, size{size} {}
};

#define Zof(T) Z_t( #T, Z(T))

int main() {
	auto zs = {
		Zof(u64), Zof(S),
		Zof(Db::EntTy), Zof(Db::Ent), Zof(Q::Q), Zof(Bc::In),
		Zof(VM::Bod), Zof(Asm::Asm), Zof(T::T),
	};
	for (auto &z : zs) {
		std::cout<<z.name<<' '<<z.size*8<<' '<<z.size<<std::endl;
	}
}
