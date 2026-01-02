#include <cstring>
#include <optional>

#include <str.h>

/* interned string table */
namespace Str::Interns {
	S cap = 16, len = 0;
	u8 **ptr = 0x0;
	std::mutex mut;

	void init() {
		ptr = new u8*[cap];
	}

	void deinit() {
		auto G = LOCK(mut);
		for (S i = 0; i < len; i++) {
			delete[] ptr[i];
		}
		delete[] ptr;
	}

	/* resize table height */
	void reZ() {
		if (len >= cap) {
			auto N = new u8*[cap*2];
			memmove(N, ptr, Z(u8*)*cap);

			auto G = LOCK(mut);
			delete[] ptr;
			ptr = N;
			cap *= 2;
		}
	}

	auto push(u8 *str) -> S {
		S L;
		u8 *heap;

		/* safety */
		reZ();

		/* make a heap str */
		L = strlen((char*)str);
		heap = new u8[L+1];
		memmove(heap, str, Z(u8)*L);
		heap[L] = 0;

		auto G = LOCK(mut);
		L = len++;
		ptr[L] = heap;
		return L;
	}

	/* find the index of a string in the interned db */
	inl auto fnd(u8 *str) -> std::optional<S> {
		for (S i = 0; i < len; i++) {
			if (strcmp((char*)str, (char*)ptr[i])==0) {
				putc('0' + i, stdout);
				return i;
			}
		}

		return {};
	}

	auto add(u8 *str) -> S {
		auto o = fnd(str);
		if (o) return *o;
		else return push(str);
	}
}


Str::A::A(const char *str) : len{strlen(str)} {
	auto ptr = new u8[len+1];
	memcpy(ptr, str, Z(u8)*(len+1));
	this->str = Interns::add(ptr);
}

Str::A::~A() {}

Str::A::A(const A &x) : len{x.len}, str{x.str} {}

auto Str::A::operator=(const A &x) -> const A& {
	len = x.len, str = x.str;
	return *this;
}

auto Str::A::cstr() -> char* {
	auto G = LOCK(Interns::mut);
	auto i = (char*)Interns::ptr[str];
	auto L = strlen(i)+1;
	auto ptr = new char[L];
	memcpy(ptr, i, Z(char)*L);
	return ptr;
}
