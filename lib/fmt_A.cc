#include <A.h>

auto Fmt::Fmt(A::A<Chr> *x) -> std::string {
	std::stringstream ss;

	ss << '"';
	for (S i = 0; i < x->len; i++) {
		ss << (*x)[i];
	}
	ss << '"';

	return ss.str();
}
