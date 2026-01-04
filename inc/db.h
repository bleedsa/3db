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
	};

	extern const char *EntTy_names[4];

	struct Ent {
		var_t name; 
		EntTy ty;
		union {
			i32 i; /* Int */
			S z;   /* Sz */
			f32 f; /* Flt */
			f64 d; /* Dbl */
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

		/* entry constructors with no EntTy parameter */
		#define EntNoTyp(T) Ent(var_t name, T x)
		EntNoTyp(i32);
		EntNoTyp(S);
		EntNoTyp(f32);
		EntNoTyp(f64);
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
	std::string Ent(Db::Ent *x);
}

#endif
