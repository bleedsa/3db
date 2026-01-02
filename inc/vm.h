#ifndef __3DB_VM_H__
#define __3DB_VM_H__

#include <mutex>
#include <vector>

#include <u.h>
#include <bc.h>

namespace VM {
	/* a simple numeric repr of a body in the vm. contains a number of vars
	 * to allocate for the body as well as a start index into the instruc-
	 * tion vector. */
	struct Bod {
		S vars, start;

		inl Bod() : vars{0}, start{0} {}
		inl Bod(S vars, S start) : vars{vars}, start{start} {}
		inl ~Bod() {}

		inl Bod(const Bod &x) : vars{x.vars}, start{x.start} {}
		inl auto operator=(const Bod &x) -> const Bod& {
			vars = x.vars, start = x.start;
			return *this;
		}
	};

	/* make a body */
	struct MkBod {
		/* var names indexed by its number */
		std::vector<std::string> vars;
		std::vector<Bc::In> ins;
		S tags;

		MkBod();
		inl ~MkBod() {}

		MkBod(const MkBod &x);
		const MkBod &operator=(const MkBod &x);
	};

	struct VM {
		Bc::In *ins;
		Bod *bods;
		S inL, bodL;

		VM(Bc::In *ins, S inL, Bod *bods, S bodL);
		~VM();

		VM(const VM &x);
		const VM &operator=(const VM &x);
	};

	using VMs = std::vector<std::tuple<S, VM>>;

	/*
	 * stuff for the vm
	 */
	extern std::mutex mut;
	extern VMs vms;
}

#endif
