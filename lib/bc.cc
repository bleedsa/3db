#include <bc.h>

Bc::In::In(InTy ty, I x)    : ty{ty}, i{x} {}
Bc::In::In(InTy ty, f64 x)  : ty{ty}, d{x} {}
Bc::In::In(InTy ty, char x) : ty{ty}, c{x} {}

Bc::In::In(InTy ty, const char *x) : ty{ty}, var{str_to_var(x)} {}
Bc::In::In(InTy ty, var_t x) : ty{ty}, var{x} {}
Bc::In::In(InTy ty, T::TColTy x) : ty{ty}, i{(i32)x} {}

Bc::In::In(I x)    : ty{Bc::LITi32}, i{x} {}
Bc::In::In(f64 x)  : ty{Bc::LITf64}, d{x} {}
Bc::In::In(char x) : ty{Bc::LITChr}, c{x} {}

Bc::In::In(var_t x) : ty{Bc::LITVar}, var{x} {}
Bc::In::In(const char *x) : ty{Bc::LITVar}, var{str_to_var(x)} {}

Bc::In::In(const In &x) {
	memcpy(this, &x, Z(Bc::In));
}

auto Bc::In::operator=(const In &x) -> const In& {
	memcpy(this, &x, Z(Bc::In));
	return *this;
}
