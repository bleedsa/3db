#include <iostream>

#include <db.h>

struct Z_t {
	const char *name;
	S size;

	Z_t(const char *name, S size) : name{name}, size{size} {}
};

#define Zof(T) Z_t( #T, 8*Z(T))

int main() {
	auto zs = {Zof(Db::EntTy), Zof(Db::Ent)};
	for (auto &z : zs) {
		std::cout << z.name << " " << z.size << std::endl;
	}
}
