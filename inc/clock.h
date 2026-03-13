#ifndef __3DB_CLOCK_H__
#define __3DB_CLOCK_H__

#include <chrono>

namespace Clk {
	using Clk = std::chrono::high_resolution_clock;

	template<typename F>
	auto time(F f) {
		auto x = Clk::now();
		f();
		auto y = Clk::now();
		return y - x;
	}
};

#endif
