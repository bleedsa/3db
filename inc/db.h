#ifndef __3DB_DB_H__
#define __3DB_DB_H__

#include <vector>
#include <string>

#include <str.h>

namespace Db {
	enum EntTy {
		Int,
		Sz,
		Flt,
		Dbl,
	};

	extern const char *EntTy_names[4];

	struct Ent {
		/* the name's index in Str::Interns */
		char *name;
		EntTy ty;
		union {
			i32 i;
			S z;
			f32 f;
			f64 d;
		};

		~Ent();
		Ent(const Ent &x);
		const Ent &operator=(const Ent &x);

		#define EntBasic(T) Ent(const char *name, EntTy ty, T x)
		EntBasic(i32);
		EntBasic(S);
		EntBasic(f32);
		EntBasic(f64);

		#define EntNoTyp(T) Ent(const char *name, T x)
		EntNoTyp(i32);
		EntNoTyp(S);
		EntNoTyp(f32);
		EntNoTyp(f64);
	};

	extern std::vector<Ent> ents;
	extern std::mutex mut;

	void push_ent(Ent x);

	template<typename X> inl void push_ent(const char *name, X x) {
		auto G = LOCK(mut);
		ents.push_back(Ent(name, x));
	}
}	

namespace Fmt {
	std::string Ent(Db::Ent *x);
}

#endif
