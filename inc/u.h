#ifndef __3DB_U_H__
#define __3DB_U_H__

#include <cctype>
#include <cstdint>
#include <string>
#include <format>
#include <iostream>
#include <expected>

#define Z(x) (sizeof(x))
#define inl __attribute__((always_inline)) inline
#define CASE(h,b...) case h: { b; break; }

#define LOCK(x) (std::lock_guard<std::mutex>(x))

#define err_fmt(f...) (std::unexpected(std::format(f)))
#define println(f...) {auto _x=std::format(f);std::cout<<_x<<std::endl;}
#define fatal(f...) { \
	auto _x=std::format("FATAL: " f); \
	std::cerr<<_x<<std::endl; \
	exit(-1); \
}

/* fuck you */
#define A_err(f...) ({ \
	auto _x=std::format(f); \
	(char*)Str::Interns::ptr[Str::Interns::add((u8*)_x.c_str())]; \
})

#ifdef DBG 
#define dbg(x...) {x;}
#else
#define dbg(x...) {}
#endif

using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;

using i8 = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;

using f32 = float;
using f64 = double;

using I = i32;
using S = size_t;
using Chr = unsigned char;

template<typename X>
using R = std::expected<X, std::string>;

namespace Three {
	void init();
	void deinit();
}

#endif
