#include <fstream>
#include <cstring>

#include <db.h>
#include <fs.h>

auto T::T::fill_buf(u8 *ptr) -> void {
	S x, z;

	/* fill with the column types */
	z = Z(TColTy)*coln; memcpy(ptr, col_tys,   z); ptr += z;
	/* fill with the column names */
	z = Z(var_t)*coln;  memcpy(ptr, col_names, z); ptr += z;

	/* fill the buffer with the columns */
	for (
		x = 0, z = colZof(x);
		x < coln;
	) {
		z = colZof(x);
		memcpy(ptr, cols[x], z);
		ptr += z;
		x++;
	}

	/* fill with the init column */
	memcpy(ptr, init, Z(bool)*row_cap);
}

auto T::T::from_buf(u8 *ptr) -> void {
	S x, z;

	/* construct */
	col_tys = mk<TColTy>(coln);
	col_names = mk<var_t>(coln);
	cols = mk<u8*>(coln);
	init = mk<bool>(row_cap);
	refs = new S;

	/* copy some column information */
	z = Z(TColTy)*coln; memmove(col_tys,   ptr, z); ptr += z;
	z = Z(var_t)*coln;  memmove(col_names, ptr, z); ptr += z;

	/* finish setting up the columns */
	for (x = 0; x < coln; x++) cols[x] = mk<u8>(colZof(x));
	*refs = 1;

	/* copy out of the ptr */
	for (
		x = 0, z = colZof(x);
		x < coln;
	) {
		z = colZof(x);
		memmove(cols[x], ptr, z);
		ptr += z;
		x++;
	}

	/* fill the init column */
	memcpy(init, ptr, Z(bool)*row_cap);
}

auto Q::Q::fill_buf_with_elems(u8 *buf) -> void {
	switch (ty) {
	CASE(QNil, {})

	/* atoms are a simple cast */
	CASE(QInt, memcpy(buf, &i, Z(i32)))
	CASE(QDbl, memcpy(buf, &d, Z(f64)))
	CASE(QChr, memcpy(buf, &c, Z(Chr)))

	/* vectors are memcpy */
	CASE(QINT, memcpy(buf, iA.ptr, Z(i32)*iA.len))
	CASE(QDBL, memcpy(buf, dA.ptr, Z(f64)*dA.len))
	CASE(QCHR, memcpy(buf, cA.ptr, Z(Chr)*cA.len))

	/* tables */
	CASE(QTab, t.fill_buf_full(buf))

	/* vectors are memcpy */
	default: fatal("trying to fill buf with Q of type {}", short_name());
	}
}

auto Q::Q::to_bytes() -> u8* {
	u64 L = len();
	u8 *buf = new u8[calc_serial_Z()], *ptr = buf;

	/* copy the type */
	(ptr++)[0] = ty;
	/* copy the length */
	memcpy(ptr, &L, Z(u64));
	/* fill */
	fill_buf_with_elems(ptr);

	return buf;
}

/* the size of the header */
constexpr S headZ = Z(var_t) /* name */
	+ Z(u64) /* len */
	+ 1 /* ty */;

/* Ent from bytes */
Db::Ent::Ent(u8 *ptr) : ty{(EntTy)(ptr++)[0]} {
	/* special case for tables */
	if (ty == Tab) {
		/* unpack the header */
		memcpy(&t.coln,    ptr, Z(u32));   ptr += Z(u32);
		memcpy(&t.row_cap, ptr, Z(u32));   ptr += Z(u32);
		memcpy(&name,      ptr, Z(var_t)); ptr += Z(var_t);

		t.from_buf(ptr);
	} else {
		u64 L = 0;

		/* get the length and name */
		memcpy(&L,    ptr, Z(u64));   ptr += Z(u64);
		memcpy(&name, ptr, Z(var_t)); ptr += Z(var_t);

		switch (ty) {
		/* atoms */
		CASE(Int, memcpy(&i, ptr, Z(i32)))
		CASE(Sz,  memcpy(&z, ptr, Z(S)))
		CASE(Flt, memcpy(&f, ptr, Z(f32)))
		CASE(Dbl, memcpy(&d, ptr, Z(f64)))
		CASE(Ch,  memcpy(&c, ptr, Z(Chr)))
		/* vecs */
		CASE(INT, iA=A::A<i32>((i32*)ptr, L))
		CASE(SZ,  zA=A::A<S>((S*)ptr, L))
		CASE(FLT, fA=A::A<f32>((f32*)ptr, L))
		CASE(DBL, dA=A::A<f64>((f64*)ptr, L))
		CASE(CHR, cA=A::A<Chr>((Chr*)ptr, L))
		default: fatal(
			"cannot construct entry of type {} from bytes", type()
		);
		}
	}

}

auto Db::Ent::to_bytes_table() -> std::tuple<u8*, u64> {
	u8 *ptr, *init;
	u64 z;

	/* set up a buffer we can write to */
	z = t.calc_serial_Z();
	ptr = mk<u8>(headZ + z), init = ptr;
	memset(ptr, 0, headZ+z);

	/* copy the header data into the buffer and inc the ptr */
	(ptr++)[0] = ty;
	memcpy(ptr, &t.coln,    Z(u32));   ptr += Z(u32);
	memcpy(ptr, &t.row_cap, Z(u32));   ptr += Z(u32);
	memcpy(ptr, &name,      Z(var_t)); ptr += Z(var_t);

	/* copy the rest of the data into the ptr */
	t.fill_buf(ptr);

	return std::tuple<u8*, u64>{init, headZ+z};
}

auto Db::Ent::to_bytes_simple() -> std::tuple<u8*, u64> {
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
	CASE(Ch,  memcpy(ptr, &c, Z(Chr)))
	/* vecs */
	CASE(INT, memcpy(ptr, iA.ptr, z))
	CASE(SZ,  memcpy(ptr, zA.ptr, z))
	CASE(FLT, memcpy(ptr, fA.ptr, z))
	CASE(DBL, memcpy(ptr, dA.ptr, z))
	CASE(CHR, memcpy(ptr, cA.ptr, z))
	default: fatal("to_bytes_simple() called on entry of type {}", type());
	}

	return std::tuple<u8*, u64>{init, headZ+z};
}

auto Db::Ent::to_bytes() -> std::tuple<u8*, u64> {
	if (ty == Tab) return to_bytes_table();
	else return to_bytes_simple();
}

auto Db::write(const char *path) -> void {
	auto G = LOCK(mut);
	auto f = Fs::Bin(path, true);
	u64 L = ents.size();

	/* write the header */
	f << L;

	/* write the entries */
	for (S i = 0; i < L; i++) {
		auto e = &Db::ents[i];
		auto [b, z] = e->to_bytes();

		/* every entry starts with the size of the buffer in bytes */
		f << z;

		/* write one byte at a time */
		for (S j = 0; j < z; j++) {
			f << b[j];
		}
	}
}

auto Db::load(const char *path) -> void {
	u64 L, z;
	u8 b, *ptr;
	auto f = Fs::Bin(path);

	/* read the header */
	f >> L;

	for (S i = 0; i < L; i++) {
		/* each entry starts with the size of the buffer in bytes */
		f >> z;
		/* make a buffer to fill */
		ptr = new u8[z];

		/* load byte by byte */
		for (S j = 0; j < z; j++) {
			f >> b;
			ptr[j] = b;
		}

		auto e = Ent(ptr);
		push_ent(e);
		delete[] ptr;
	}
}
