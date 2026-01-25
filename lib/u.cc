#include <u.h>
#include <str.h>
#include <db.h>
#include <sse.h>

void Three::init() {
	Str::Interns::init();
}

void Three::deinit() {
	Str::Interns::deinit();
	Db::deinit();
}

auto U::sum_vec(S *v, S n) -> S {
	S i, L, off, r = 0;
	/* sum accumulator */
	xmm64_t s = {0, 0};

	/* add all the vectors to the sum accumulator */
	for (i = 0, L = n / 2, off = 0; i < L; i++, off = i * 2)
		s += *(xmm64_t*)(v+off);

	/* sum the accumulators */
	r = s[0] + s[1];

	/* add the remainder */
	for (i = n / 2 * 2; i < n; i++) r += v[i];

	return r;
}
