#include <sstream>

#include <Q.h>

auto Fmt::Q(Q::Q *x) -> std::string {
	std::stringstream ss;

	switch (x->ty) {
	CASE(Q::QInt, ss<<x->i)
	CASE(Q::QSz,  ss<<x->z)
	CASE(Q::QFlt, ss<<x->f)
	CASE(Q::QDbl, ss<<x->d)
	}

	return ss.str();
}
