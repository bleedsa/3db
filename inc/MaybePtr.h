#ifndef __3DB_MAYBEPTR_H__
#define __3DB_MAYBEPTR_H__

template<typename X>
struct MaybePtr {
	bool is_owned;
	union {
		X own;
		X *ptr;
	};

	inl MaybePtr(X x) : is_owned{true}, own{x} {}
	inl MaybePtr(X *P) : is_owned{false}, ptr{P} {}

	~MaybePtr() {
//		if (is_owned) own.~X();
	}

	inl auto cpy(const MaybePtr &x) -> void {
		if (is_owned) own = x.own;
		else ptr = x.ptr;
	}

	inl MaybePtr(const MaybePtr &x) {cpy(x);}
	inl MaybePtr &operator=(const MaybePtr &x) {cpy(x);return *this;}

	inl auto operator*() -> X* {
		return is_owned ? &own : ptr;
	}
};

#endif
