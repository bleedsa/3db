#include <db.h>

namespace Db {
	std::mutex mut;
	std::vector<Ent> ents;

	const char *EntTy_names[] = {
		"i32", "usize", "f32", "f64", "chr",
		"I32", "USIZE", "F32", "F64", "CHR",
		"table"
	};

	const S EntTy_Z[] = {
		Z(i32), Z(S), Z(f32), Z(f64), Z(Chr),
		Z(i32), Z(S), Z(f32), Z(f64), Z(Chr),
		Z(T::T),
	};
}

auto Db::deinit() -> void {}

Db::Ent::~Ent() {
	switch (ty) {
	CASE(INT, this->iA.~A())
	CASE(SZ,  this->zA.~A())
	CASE(FLT, this->fA.~A())
	CASE(DBL, this->dA.~A())
	CASE(CHR, this->cA.~A())
	CASE(Tab, this->t.~T())
	default: {}
	}
}

inl auto ent_cpy_value(Db::Ent *x, Db::Ent *y) -> void {
	x->name = y->name;
	switch (y->ty) {
	/* atoms */
	CASE(Db::Int,x->i=y->i)
	CASE(Db::Sz, x->z=y->z)
	CASE(Db::Flt,x->f=y->f)
	CASE(Db::Dbl,x->d=y->d)
	CASE(Db::Ch, x->c=y->c)
	/* vectors */
	CASE(Db::INT,x->iA=y->iA)
	CASE(Db::SZ, x->zA=y->zA)
	CASE(Db::FLT,x->fA=y->fA)
	CASE(Db::DBL,x->dA=y->dA)
	CASE(Db::CHR,x->cA=y->cA)
	/* misc */
	CASE(Db::Tab,x->t=y->t)
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
EntBasicImpl(Chr,c);

EntBasicImpl(A::A<i32>, iA);
EntBasicImpl(A::A<S>,   zA);
EntBasicImpl(A::A<f32>, fA);
EntBasicImpl(A::A<f64>, dA);
EntBasicImpl(A::A<Chr>, cA);
EntBasicImpl(T::T,      t );

#define EntNoTypImpl(X,T,f) \
	Db::Ent::Ent(var_t name, X x) : name{name}, ty{T}, f{x} {}
EntNoTypImpl(i32,Int,i);
EntNoTypImpl(S,  Sz, z);
EntNoTypImpl(f32,Flt,f);
EntNoTypImpl(f64,Dbl,d);
EntNoTypImpl(Chr,Ch, c);

EntNoTypImpl(A::A<i32>,INT,iA);
EntNoTypImpl(A::A<S>,  SZ, zA);
EntNoTypImpl(A::A<f32>,FLT,fA);
EntNoTypImpl(A::A<f64>,DBL,dA);
EntNoTypImpl(A::A<Chr>,CHR,cA);
EntNoTypImpl(T::T,     Tab,t );

inl auto Db_get_idx(var_t name) -> std::optional<S> {
	S r, i = 0;
	for (auto &e : Db::ents) {
		if ((r = std::memcmp(&name, &e.name, Z(var_t))) == 0) {
			return i;
		}
		i++;
	}
	return {};
}

auto Db::get(var_t name) -> std::optional<Ent*> {
	S r;
	for (auto &e : ents) {
		if ((r = std::memcmp(&name, &e.name, Z(var_t))) == 0) {
			return &e;
		}
	}
	return {};
}

auto Db::del(var_t name) -> char* {
	auto o = Db_get_idx(name);
	if (!o) {
		auto s = var_to_str(name);
		auto e = A_err("entry {} not found", s);
		free(s);
		return e;
	}
	auto i = *o;
	auto G = LOCK(mut);
	ents.erase(ents.begin() + i);
	return nullptr;
}
