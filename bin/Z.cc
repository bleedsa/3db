#include <iostream>

#include <Q.h>
#include <db.h>
#include <bc.h>
#include <vm.h>

struct Z_t {
	const char *name;
	S size;

	Z_t(const char *name, S size) : name{name}, size{size} {}
};

#define Zof(T) Z_t( #T, 8*Z(T))

int main() {
	auto zs = {
		Zof(Db::EntTy), Zof(Db::Ent), Zof(Q::Q), Zof(Bc::In),
		Zof(VM::Bod),
	};
	for (auto &z : zs) {
		std::cout << z.name << " " << z.size << std::endl;
	}
}
