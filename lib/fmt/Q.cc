#include <sstream>

#include <Q.h>

auto Fmt::Fmt(Q::Q *x) -> std::string {
	std::stringstream ss;

	switch (x->ty) {
	/* atoms */
	CASE(Q::QNil, ss<<"nil")
	CASE(Q::QInt, ss<<x->i)
	CASE(Q::QDbl, ss<<x->d)
	CASE(Q::QChr, ss<<x->c)
	/* vecs */
	CASE(Q::QINT, ss<<Fmt::Fmt(&x->iA))
	CASE(Q::QDBL, ss<<Fmt::Fmt(&x->dA))
	CASE(Q::QCHR, ss<<Fmt::Fmt(&x->cA))
	/* table */
	CASE(Q::QTab, ss<<Fmt::Fmt(&x->t))
	/* vars */
	CASE(Q::QVar, {
		auto str = var_to_str(x->var);
		ss << str;
		delete[] str;
	})

	default: ss << '{' << x->short_name() << "???}";
	}

	return ss.str();
}
