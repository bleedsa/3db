#include <sstream>

#include <Q.h>

auto Fmt::Fmt(Q::Q *x) -> std::string {
	std::stringstream ss;

	switch (x->ty) {
	/* atoms */
	CASE(Q::QNil, ss<<"nil")
	CASE(Q::QInt, ss<<x->i)
	CASE(Q::QSz,  ss<<x->z)
	CASE(Q::QFlt, ss<<x->f)
	CASE(Q::QDbl, ss<<x->d)
	/* vecs */
	CASE(Q::QINT, ss<<Fmt::Fmt(&x->iA))
	CASE(Q::QSZ,  ss<<Fmt::Fmt(&x->zA))
	CASE(Q::QFLT, ss<<Fmt::Fmt(&x->fA))
	CASE(Q::QDBL, ss<<Fmt::Fmt(&x->dA))

	default: ss << '{' << (u8)x->ty << "???}";
	}

	return ss.str();
}
