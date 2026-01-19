#ifndef __3DB_SSE_H__
#define __3DB_SSE_H__

#include <u.h>

typedef u64 xmm64_t __attribute__((vector_size(16)));

namespace SSE {
	/* fuse two u64 into one xmm64_t */
	inl auto xmm64_fuse(u64 x, u64 y) -> xmm64_t {
		xmm64_t r;
		u64 *ptr;

		ptr = (u64*)&r;
		memmove(ptr, &x, Z(u64)); ptr++;
		memmove(ptr, &y, Z(u64));

		return r;
	}

	/* unfuse an xmm64_t into two u64s */
	inl auto xmm64_unfuse(xmm64_t x64) -> std::tuple<u64, u64> {
		u64 x, y, *ptr = (u64*)&x64;

		memmove(&x, ptr, Z(u64)); ptr++;
		memmove(&y, ptr, Z(u64));

		return {x, y};
	}
}

#endif
