#include <cstdlib>

#include <AsmParser.h>

auto Asm::esc(char c) -> std::string {
	if (c == EOF) return "EOF";
	else if (c == 0) return "\\0";
	else {
		auto s = std::string();
		s.push_back(c);
		return s;
	}
}

auto Asm::AsmParser::size() -> R<S> {
	S L, r;
	char *start = src, *num;
	
	for (L = 0; *src && isdigit(*src); src++, L++);
	if (L == 0) return err_fmt("expected integer (size_t), got {}", *src);

	num = new char[L+1];
	memcpy(num, start, L*Z(char));
	num[L] = 0;

	r = atoll((const char*)num);

	return r;
}

auto Asm::AsmParser::body_header() -> R<VM::Bod> {
	S vars, start;

	auto z = size();
	if (!z) return err_fmt("expected vars... {}", z.error());
	vars = *z;

	if (!expect(',')) return err_fmt("expected ',', got {}", esc(*src));

	z = size();
	if (!z) return err_fmt(
		"expected instruction start position... {}", z.error()
	);
	start = *z;

	return VM::Bod(vars, start);
}

auto Asm::AsmParser::in_get() -> bool {
	if (!expect("GET")) return false;

	return true;
}
