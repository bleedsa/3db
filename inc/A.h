#ifndef __3DB_A_H__
#define __3DB_A_H__

#include <sstream>
#include <cstring>

#include <mem.h>

namespace A {
	/* a static width, memcpy'd vector */
	template<typename X>
	struct A {
		S len;
		X *ptr;

		inl A(S len) : len{len}, ptr{mk<X>(len)} {}
		inl ~A() {free(ptr);}

		A(std::initializer_list<X> x) : len{x.size()}, ptr{mk<X>(len)} {
			S i = 0;
			for (auto e : x) {
				ptr[i++] = e;
			}
		}

		A(X *buf, S len) : len{len}, ptr{mk<X>(len)} {
			memcpy(ptr, buf, Z(X)*len);
		}

		inl A(const A &x) : len{x.len}, ptr{mk<X>(len)} {
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

		template<typename Y, typename F>
		inl auto each(F f) -> A<Y> {
			auto a = A<Y>(len);
			for (S i = 0; i < len; i++) a[i] = f(ptr+i);
			return a;
		}

		template<typename F>
		inl auto for_each(F f) -> void {
			for (S i = 0; i < len; i++) f(ptr+i);
		}
	};
}

namespace Fmt {
	std::string Fmt(A::A<Chr> *x);

	template<typename X>
	static auto Fmt(A::A<X> *x) -> std::string {
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
