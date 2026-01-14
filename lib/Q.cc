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
	Z(i32),
	Z(f64),
	Z(Chr),
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

	/* memcpy vecs */
	CASE(QINT, iA=A::A((i32*)ptr, L))
	CASE(QDBL, dA=A::A((f64*)ptr, L))
	CASE(QCHR, cA=A::A((Chr*)ptr, L))

	default: fatal("raw Q::Q::Q with type {}", (S)ty);
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

auto Q::Q::fill_buf_with_elems(u8 *buf) -> void {
	switch (ty) {
	CASE(QNil, {})

	/* atoms are a simple cast */
	CASE(QInt, memcpy(buf, &i, Z(i32)))
	CASE(QDbl, memcpy(buf, &d, Z(f64)))
	CASE(QChr, memcpy(buf, &c, Z(Chr)))

	/* vectors are memcpy */
	CASE(QINT, memcpy(buf, iA.ptr, Z(i32)*iA.len))
	CASE(QDBL, memcpy(buf, dA.ptr, Z(f64)*dA.len))
	CASE(QCHR, memcpy(buf, cA.ptr, Z(Chr)*cA.len))

	/* vectors are memcpy */
	default: fatal("Q::Q::fill_buf_with_elems() on Q of type {}", (S)ty);
	}
}
