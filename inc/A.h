#ifndef __3DB_A_H__
#define __3DB_A_H__

#include <sstream>

#include <mem.h>

namespace A {
	template<typename X>
	struct A {
		X *ptr;
		S len;

		inl A(S len) : ptr{mk<X>(len)}, len{len} {}
		inl ~A() {free(ptr);}

		inl A(const A &x) : ptr{mk<X>(x.len)}, len{x.len} {
			memcpy(ptr, x.ptr, Z(X)*len);
		}

		inl auto operator=(const A &x) -> const A& {
			len = x.len, ptr = mk<X>(len);
			memcpy(ptr, x.ptr, Z(X)*len);
			return *this;
		}

		inl auto operator[](int i) -> X& {
			return ptr[i];
		}
	};
}

namespace Fmt {
	template<typename X>
	static auto A(A::A<X> *x) -> std::string {
		std::stringstream ss;
		S L = x->len - 1;

		ss << '(';
		for (S i = 0; i < L; i++) {
			ss << x->ptr[i] << ';';
		}
		ss << x->ptr[L] << ')';

		return ss.str();
	}
}

#endif
