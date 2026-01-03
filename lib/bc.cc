#include <bc.h>

Bc::In::In(InTy ty, I x)   : ty{ty}, i{x} {}
Bc::In::In(InTy ty, S x)   : ty{ty}, z{x} {}
Bc::In::In(InTy ty, f32 x) : ty{ty}, f{x} {}
Bc::In::In(InTy ty, f64 x) : ty{ty}, d{x} {}

Bc::In::In(I x)   : ty{Bc::LITi32}, i{x} {}
Bc::In::In(S x)   : ty{Bc::LITSz},  z{x} {}
Bc::In::In(f32 x) : ty{Bc::LITf32}, f{x} {}
Bc::In::In(f64 x) : ty{Bc::LITf64}, d{x} {}

#define IN_CLN() { \
	ty=x.ty; \
	switch (ty) { \
	CASE(Bc::LITi32, i=x.i) \
	CASE(Bc::LITSz,  z=x.z) \
	CASE(Bc::LITf32, f=x.f) \
	CASE(Bc::LITf64, d=x.d) \
	case LOAD: \
	case STORE: \
		z = x.z; \
		break; \
	default: fatal("could not copy In of type {}", (u8)ty); \
	} \
}

Bc::In::In(const In &x) {
	IN_CLN();
}

auto Bc::In::operator=(const In &x) -> const In& {
	IN_CLN();
	return *this;
}
