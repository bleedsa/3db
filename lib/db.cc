#include <db.h>

namespace Db {
	std::mutex mut;
	std::vector<Ent> ents;
	const char *EntTy_names[] = {"int", "usize", "f32", "f64"};
}

Db::Ent::~Ent() {}

inl auto ent_cpy_value(Db::Ent *x, Db::Ent *y) -> void {
	x->name = y->name;
	switch (y->ty) {
	CASE(Db::Int,x->i=y->i)
	CASE(Db::Sz, x->z=y->z)
	CASE(Db::Flt,x->f=y->f)
	CASE(Db::Dbl,x->d=y->d)
	}
}

Db::Ent::Ent(const Ent &x) : ty{x.ty} {
	ent_cpy_value((Ent*)this, (Ent*)&x);
}

auto Db::Ent::operator=(const Ent &x) -> const Ent& {
	ty = x.ty;
	ent_cpy_value((Ent*)this, (Ent*)&x);
	return *this;
}

/* make a basic ent constructor with value type T and field name f.
 * adds entry name to the interned string table. */
#define EntBasicImpl(T,f) \
	Db::Ent::Ent(var_t name, EntTy ty, T x) : name{name}, ty{ty}, f{x} {}
EntBasicImpl(i32,i);
EntBasicImpl(S,  z);
EntBasicImpl(f32,f);
EntBasicImpl(f64,d);

#define EntNoTypImpl(X,T,f) \
	Db::Ent::Ent(var_t name, X x) : name{name}, ty{T}, f{x} {}
EntNoTypImpl(i32,Int,i);
EntNoTypImpl(S,  Sz, z);
EntNoTypImpl(f32,Flt,f);
EntNoTypImpl(f64,Dbl,d);

auto Db::get(var_t name) -> std::optional<Ent*> {
	S r;
	for (auto &e : ents) {
		if ((r = std::memcmp(&name, &e.name, Z(var_t))) == 0) return &e;
	}
	return {};
}
