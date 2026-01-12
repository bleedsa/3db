#ifndef __3DB_DB_H__
#define __3DB_DB_H__

#include <vector>
#include <string>

#include <str.h>
#include <bc.h>

namespace Db {
	enum EntTy {
		Int,
		Sz,
		Flt,
		Dbl,
		INT,
		SZ,
		FLT,
		DBL,
	};

	extern const char *EntTy_names[8];
	extern const S EntTy_Z[8];

	struct Ent {
		var_t name; 
		EntTy ty;
		union {
			i32 i; /* Int */
			S z;   /* Sz */
			f32 f; /* Flt */
			f64 d; /* Dbl */
			A::A<i32> iA;
			A::A<S> zA;
			A::A<f32> fA;
			A::A<f64> dA;
		};

		~Ent();
		Ent(const Ent &x);
		const Ent &operator=(const Ent &x);

		/* basic Ent constructors */
		#define EntBasic(T) Ent(var_t name, EntTy ty, T x)
		EntBasic(i32);
		EntBasic(S);
		EntBasic(f32);
		EntBasic(f64);
		EntBasic(A::A<i32>);
		EntBasic(A::A<S>);
		EntBasic(A::A<f32>);
		EntBasic(A::A<f64>);

		/* entry constructors with no EntTy parameter */
		#define EntNoTyp(T) Ent(var_t name, T x)
		EntNoTyp(i32);
		EntNoTyp(S);
		EntNoTyp(f32);
		EntNoTyp(f64);
		EntNoTyp(A::A<i32>);
		EntNoTyp(A::A<S>);
		EntNoTyp(A::A<f32>);
		EntNoTyp(A::A<f64>);

		inl auto atom_size() -> S {
			return EntTy_Z[ty];
		}

		inl auto len() -> S {
			switch (ty) {
			case Int:
			case Sz:
			case Flt:
			case Dbl:
				return 1;
			CASE(INT, return iA.len)
			CASE(SZ,  return zA.len)
			CASE(FLT, return fA.len)
			CASE(DBL, return dA.len)
			default: fatal("len() on entry of type {}", (S)ty);
			}
		}

		Ent(u8 *ptr);
		std::tuple<u8*, S> to_bytes();
	};

	extern std::vector<Ent> ents;
	extern std::mutex mut;

	/* push a raw entry to the database */
	inl auto push_ent(Ent x) -> void {
		auto G = LOCK(mut);
		ents.push_back(x);
	}

	/* push an entry onto the db and figure out the type based on the
	 * value type */
	template<typename X>
	inl void push_ent(var_t name, X x) {
		auto G = LOCK(mut);
		ents.push_back(Ent(name, x));
	}

	template<typename X>
	inl void push_ent(const char *name, X x) {
		auto G = LOCK(mut);
		auto v = str_to_var(name);
		ents.push_back(Ent(v, x));
	}

	std::optional<Ent*> get(var_t name);

	inl auto get(const char *name) -> std::optional<Ent*> {
		auto v = str_to_var(name);
		return get(v);
	}
}	

namespace Fmt {
	std::string Fmt(Db::Ent *x);
}

#endif
