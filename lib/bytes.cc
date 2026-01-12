#include <db.h>

constexpr S headZ = Z(var_t) + Z(u64) + 1;

/* Ent from bytes */
Db::Ent::Ent(u8 *ptr) : ty{(EntTy)(ptr++)[0]} {
	u64 L = 0;

	/* get the length */
	memcpy(&L,    ptr, Z(u64));   ptr += Z(u64);
	memcpy(&name, ptr, Z(var_t)); ptr += Z(var_t);

	switch (ty) {
	/* atoms */
	CASE(Int, memcpy(&i, ptr, Z(i32)))
	CASE(Sz,  memcpy(&z, ptr, Z(S)))
	CASE(Flt, memcpy(&f, ptr, Z(f32)))
	CASE(Dbl, memcpy(&d, ptr, Z(f64)))
	/* vecs */
	CASE(INT, iA=A::A<i32>((i32*)ptr, L))
	CASE(SZ,  zA=A::A<S>((S*)ptr, L))
	CASE(FLT, fA=A::A<f32>((f32*)ptr, L))
	CASE(DBL, dA=A::A<f64>((f64*)ptr, L))
	}
}

auto Db::Ent::to_bytes() -> std::tuple<u8*, S> {
	u8 *ptr, *init;
	u64 L, z;

	L = len(), z = atom_size() * L;
	ptr = mk<u8>(headZ + z), init = ptr;

	(ptr++)[0] = ty;
	memcpy(ptr, &L, Z(u64));      ptr += Z(u64);
	memcpy(ptr, &name, Z(var_t)); ptr += Z(var_t);

	switch (ty) {
	/* atoms */
	CASE(Int, memcpy(ptr, &i, Z(i32)))
	CASE(Sz,  memcpy(ptr, &z, Z(S)))
	CASE(Flt, memcpy(ptr, &f, Z(f32)))
	CASE(Dbl, memcpy(ptr, &d, Z(f64)))
	/* vecs */
	CASE(INT, memcpy(ptr, iA.ptr, z))
	CASE(SZ,  memcpy(ptr, zA.ptr, z))
	CASE(FLT, memcpy(ptr, fA.ptr, z))
	CASE(DBL, memcpy(ptr, dA.ptr, z))
	default: fatal("to_bytes() called on entry of type {}", (S)ty);
	}

	return std::tuple<u8*, S>{init, z};
}
