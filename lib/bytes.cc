#include <fstream>

#include <db.h>
#include <fs.h>

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
	default: fatal("cannot construct entry of type {} from bytes", (S)ty);
	}

}

auto Db::Ent::to_bytes() -> std::tuple<u8*, u64> {
	u8 *ptr, *init;
	u64 L, z;

	/* set up a buffer we can write to */
	L = len(), z = atom_size() * L;
	ptr = mk<u8>(headZ + z), init = ptr;
	memset(ptr, 0, headZ+z);

	/* copy the header data into the buffer and inc the ptr */
	(ptr++)[0] = ty;
	memcpy(ptr, &L, Z(u64));      ptr += Z(u64);
	memcpy(ptr, &name, Z(var_t)); ptr += Z(var_t);

	/* copy the rest of the data into the ptr */
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

	return std::tuple<u8*, u64>{init, headZ+z};
}

auto Db::write(const char *path) -> void {
	auto G = LOCK(mut);
	auto f = Fs::Bin(path, true);
	u64 L = ents.size();

	/* write the header */
	f << L;

	dbg(std::cout << "writing " << L << " entries...");
	dbg(fflush(stdout));

	/* write the entries */
	for (S i = 0; i < L; i++) {
		auto e = &Db::ents[i];
		auto [b, z] = e->to_bytes();

		/* every entry starts with the size of the buffer in bytes */
		f << z;
		dbg(std::cout << '(' << z << " bytes) ");

		/* write one byte at a time */
		for (S j = 0; j < z; j++) {
			f << b[j];
			dbg(std::cout << (char)('0'+b[j]) << ' ');
		}
	}

	dbg(std::cout << "ok" << std::endl);
}

auto Db::load(const char *path) -> void {
	u64 L, z;
	u8 b, *ptr;
	auto f = Fs::Bin(path);

	/* read the header */
	f >> L;

	dbg(std::cout << "loading " << L << " entries...");
	dbg(fflush(stdout));

	for (S i = 0; i < L; i++) {
		/* each entry starts with the size of the buffer in bytes */
		f >> z;
		/* make a buffer to fill */
		ptr = new u8[z];
		dbg(std::cout << '(' << z << " bytes) ");

		/* load byte by byte */
		for (S j = 0; j < z; j++) {
			f >> b;
			ptr[j] = b;
			dbg(std::cout << (char)('0'+b) << ' ');
		}

		auto e = Ent(ptr);
		dbg({
			auto n = var_to_str(e.name);
			std::cout << n << ' ';
			free(n);
		});
		push_ent(e);
		delete[] ptr;
	}

	dbg(std::cout << "ok" << std::endl);
}
