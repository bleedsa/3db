#ifndef __3DB_ASMPARSER_H__
#define __3DB_ASMPARSER_H__

#include <vector>

#include <bc.h>
#include <vm.h>

namespace Asm {
	std::string esc(char c);

	struct AsmParser {
		char *src;
		std::vector<Bc::In> ins;
		std::vector<VM::Bod> bods;

		inl AsmParser(std::string src)
			: src{(char*)src.c_str()}
			, ins{std::vector<Bc::In>()}
			, bods{std::vector<VM::Bod>()}
		{}
		inl ~AsmParser() {}

		/* monads */
		inl auto expect(char c) -> bool {
			return *src++ == c;
		}

		inl auto expect(const char *x) -> bool {
			for (S i = 0; i < strlen(x); i++) {
				if (!expect(x[i])) return false;
			}
			return true;
		}

		/* atom parsers */
		R<S> size();
		R<i64> integer();

		/* construction parsers */
		R<VM::Bod> body_header();
		bool in_get();
		bool in_set();
		bool body();
	};
}

#endif
