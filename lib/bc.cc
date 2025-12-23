#include <bc.h>

Bc::In::In(InTy ty, Str::A str) : ty{ty}, str{str} {}
Bc::In::~In() {}

Bc::In::In(const In &x) : ty{x.ty} {
	switch (ty) {
	case LOAD:
	case STORE:
		str = x.str;
		break;
	default: fatal("could not copy In of type {}", (u8)ty);
	}
}
