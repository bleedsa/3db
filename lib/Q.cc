#include <Q.h>

const char *Q::QTy_short[] = {
	"0",
	"i",
	"d",
	"c",
	"I",
	"D",
	"C",
	"t",
	"v",
};

const S Q::QTy_Z[] = {
	0,
	Z(i32),
	Z(f64),
	Z(Chr),
	Z(u64) + Z(i32*),
	Z(u64) + Z(f64*),
	Z(u64) + Z(Chr*),
	Z(T::T),
	Z(var_t),
};

Q::Q::Q(QTy ty, u8 *ptr, S L) : ty{ty} {
	switch (ty) {
	CASE(QNil, {})

	/* cast atoms */
	CASE(QInt, i=*(i32*)ptr)
	CASE(QDbl, d=*(f64*)ptr)
	CASE(QChr, c=*(Chr*)ptr)
	CASE(QVar, var=*(var_t*)ptr)

	/* construct vecs */
	CASE(QINT, iA=A::A((i32*)ptr, L))
	CASE(QDBL, dA=A::A((f64*)ptr, L))
	CASE(QCHR, cA=A::A((Chr*)ptr, L))

	/* tables have their own thing */
	CASE(QTab, t.from_buf_full(ptr))

	default: fatal("raw Q::Q::Q with type {}", short_name());
	}
}

Q::Q::~Q() {
	switch (ty) {
	/* only vecs need to be destroyed */
	CASE(QINT, iA.~A())
	CASE(QDBL, dA.~A())
	CASE(QCHR, cA.~A())
	CASE(QTab, t.~T())
	default: {}
	}
}

inl auto Q_cpy_val(Q::Q *x, Q::Q *y) -> void {
	switch (y->ty) {
	CASE(Q::QNil, {})
	/* atoms */
	CASE(Q::QInt, x->i=y->i)
	CASE(Q::QDbl, x->d=y->d)
	CASE(Q::QChr, x->c=y->c)
	/* vecs */
	CASE(Q::QINT, x->iA=y->iA)
	CASE(Q::QDBL, x->dA=y->dA)
	CASE(Q::QCHR, x->cA=y->cA)
	/* other */
	CASE(Q::QTab, x->t=y->t)
	CASE(Q::QVar, x->var=y->var)
	default: fatal("cannot copy Q of type {}", y->short_name());
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

