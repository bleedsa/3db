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

const S Q::QTy_Z[] = {
	0,
	Z(i32),
	Z(S),
	Z(f32),
	Z(f64),
	Z(i32),
	Z(S),
	Z(f32),
	Z(f64),
};

Q::Q::Q(QTy ty, u8 *ptr, S L) : ty{ty} {
	switch (ty) {
	CASE(QNil, {})

	/* cast atoms */
	CASE(QInt, i=*(i32*)ptr)
	CASE(QSz,  z=*(S*)ptr)
	CASE(QFlt, f=*(f32*)ptr)
	CASE(QDbl, d=*(f64*)ptr)

	/* memcpy vecs */
	CASE(QINT, iA=A::A((i32*)ptr, L))
	CASE(QSZ,  zA=A::A((S*)ptr, L))
	CASE(QFLT, fA=A::A((f32*)ptr, L))
	CASE(QDBL, dA=A::A((f64*)ptr, L))

	default: fatal("raw Q::Q::Q with type {}", (S)ty);
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
	CASE(QNil, fatal("attempting to fill buffer with nil"))

	/* atoms are a simple cast */
	CASE(QInt, memcpy(buf, &i, Z(i32)))
	CASE(QSz,  memcpy(buf, &z, Z(S)))
	CASE(QFlt, memcpy(buf, &f, Z(f32)))
	CASE(QDbl, memcpy(buf, &d, Z(f64)))

	/* vectors are memcpy */
	CASE(QINT, memcpy(buf, iA.ptr, Z(i32)*iA.len))
	CASE(QSZ,  memcpy(buf, zA.ptr, Z(S)*zA.len))
	CASE(QFLT, memcpy(buf, fA.ptr, Z(f32)*fA.len))
	CASE(QDBL, memcpy(buf, dA.ptr, Z(f64)*dA.len))

	/* vectors are memcpy */
	default: fatal("Q::Q::fill_buf_with_elems() on Q of type {}", (S)ty);
	}
}
