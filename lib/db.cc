#include <db.h>

const char *Db::EntTy_names[] = {"int", "usize", "f32", "f64"};

Db::Ent::~Ent() {}

inl auto ent_cpy_value(Db::Ent *x, Db::Ent *y) -> void {
	switch (y->ty) {
	CASE(Db::Int,x->i=y->i)
	CASE(Db::Sz, x->z=y->z)
	CASE(Db::Flt,x->f=y->f)
	CASE(Db::Dbl,x->d=y->d)
	}
}

Db::Ent::Ent(const Ent &x) : name{x.name}, ty{x.ty} {
	ent_cpy_value((Ent*)this, (Ent*)&x);
}

auto Db::Ent::operator=(const Ent &x) -> const Ent& {
	name = x.name, ty = x.ty;
	ent_cpy_value((Ent*)this, (Ent*)&x);
	return *this;
}

/* make a basic ent constructor with value type T and field name f.
 * adds entry name to the interned string table. */
#define EntBasicImpl(T,f) \
	Db::Ent::Ent(const char *name, EntTy ty, T x) : ty{ty}, f{x} { \
		auto p = Str::Interns::ptr[Str::Interns::add((u8*)name)]; \
		this->name = (char*)p; \
	}
EntBasicImpl(i32,i);
EntBasicImpl(S,  z);
EntBasicImpl(f32,f);
EntBasicImpl(f64,d);

#define EntNoTypImpl(X,T,f) \
	Db::Ent::Ent(const char *name, X x) : ty{T}, f{x} { \
		auto p = Str::Interns::ptr[Str::Interns::add((u8*)name)]; \
		this->name = (char*)p; \
	}
EntNoTypImpl(i32,Int,i);
EntNoTypImpl(S,  Sz, z);
EntNoTypImpl(f32,Flt,f);
EntNoTypImpl(f64,Dbl,d);

namespace Db {
	std::mutex mut;
	auto ents = std::vector<Ent>();

	void push_ent(Ent x) {
		ents.push_back(x);
	}
}
