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
		Ch,
		INT,
		SZ,
		FLT,
		DBL,
		CHR,
		Tab,
	};

	extern const char *EntTy_names[11];
	extern const S EntTy_Z[11];

	struct Ent {
		var_t name; 
		EntTy ty;
		union {
			i32 i; /* Int */
			S z;   /* Sz */
			f32 f; /* Flt */
			f64 d; /* Dbl */
			Chr c; /* Chr */
			A::A<i32> iA;
			A::A<S> zA;
			A::A<f32> fA;
			A::A<f64> dA;
			A::A<Chr> cA;
			T::T t;
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
		EntBasic(Chr);
		EntBasic(A::A<i32>);
		EntBasic(A::A<S>);
		EntBasic(A::A<f32>);
		EntBasic(A::A<f64>);
		EntBasic(A::A<Chr>);
		EntBasic(T::T);

		/* entry constructors with no EntTy parameter */
		#define EntNoTyp(T) Ent(var_t name, T x)
		EntNoTyp(i32);
		EntNoTyp(S);
		EntNoTyp(f32);
		EntNoTyp(f64);
		EntNoTyp(Chr);
		EntNoTyp(A::A<i32>);
		EntNoTyp(A::A<S>);
		EntNoTyp(A::A<f32>);
		EntNoTyp(A::A<f64>);
		EntNoTyp(A::A<Chr>);
		EntNoTyp(T::T);

		inl auto type() -> const char* {
			return EntTy_names[(S)ty];
		}

		inl auto atom_size() -> S {
			return EntTy_Z[ty];
		}

		inl auto len() -> S {
			switch (ty) {
			case Int:
			case Sz:
			case Flt:
			case Dbl:
			case Ch:
				return 1;
			CASE(INT, return iA.len)
			CASE(SZ,  return zA.len)
			CASE(FLT, return fA.len)
			CASE(DBL, return dA.len)
			CASE(CHR, return cA.len)
			CASE(Tab, return t.row_cap)
			default: fatal("len() on entry of type {}", type());
			}
		}

		Ent(u8 *ptr);
		std::tuple<u8*, u64> to_bytes_simple();
		std::tuple<u8*, u64> to_bytes_table();
		std::tuple<u8*, u64> to_bytes();
	};

	extern std::vector<Ent> ents;
	extern std::mutex mut;

	std::optional<Ent*> get(var_t name);

	inl auto get(const char *name) -> std::optional<Ent*> {
		auto v = str_to_var(name);
		return get(v);
	}

	char *del(var_t name);

	inl auto del(const char *name) -> char* {
		auto v = str_to_var(name);
		return del(v);
	}

	/* push a raw entry to the database */
	inl auto push_ent(Ent x) -> void {
		auto o = get(x.name);
		auto G = LOCK(mut);
		if (!o) {
			ents.push_back(x);
		} else {
			auto ptr = *o;
			ptr->~Ent();
			*ptr = x;
		}
	}

	/* push an entry onto the db and figure out the type based on the
	 * value type */
	template<typename X>
	inl void push_ent(var_t name, X x) {
		push_ent(Ent(name, x));
	}

	template<typename X>
	inl void push_ent(const char *name, X x) {
		auto v = str_to_var(name);
		push_ent(v, x);
	}

	/* write the database to a path on disk */
	void write(const char *path);

	inl auto write(std::string path) -> void {
		write(path.c_str());
	}

	/* read the database from a path on disk */
	void load(const char *path);

	inl auto load(std::string path) -> void {
		load(path.c_str());
	}

	void deinit();
}	

namespace Fmt {
	std::string Fmt(Db::Ent *x);
}

#endif
