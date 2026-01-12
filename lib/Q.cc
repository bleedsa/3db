#include <Q.h>

const char *Q::QTy_short[] = {
	"0",
	"i",
	"z",
	"f",
	"d",
	"I",
	"Z",
	"F",
	"D",
};

inl auto Q_cpy_val(Q::Q *x, Q::Q *y) -> void {
	switch (y->ty) {
	/* atoms */
	CASE(Q::QInt, x->i=y->i)
	CASE(Q::QSz,  x->z=y->z)
	CASE(Q::QFlt, x->f=y->f)
	CASE(Q::QDbl, x->d=y->d)
	/* vecs */
	CASE(Q::QINT, x->iA=y->iA)
	CASE(Q::QSZ,  x->zA=y->zA)
	CASE(Q::QFLT, x->fA=y->fA)
	CASE(Q::QDBL, x->dA=y->dA)
	default: {}
	}
}

Q::Q::~Q() {
	switch (ty) {
	/* only vecs need to be destroyed */
	CASE(QINT, iA.~A())
	CASE(QSZ,  zA.~A())
	CASE(QFLT, fA.~A())
	CASE(QDBL, dA.~A())
	default: {}
	}
}

Q::Q::Q(const Q &x) : ty{x.ty} {
	Q_cpy_val((Q*)this, (Q*)&x);
}

auto Q::Q::operator=(const Q &x) -> const Q& {
	ty = x.ty;
	Q_cpy_val((Q*)this, (Q*)&x);
	return *this;
}
