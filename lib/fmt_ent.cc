#include <sstream>

#include <db.h>
#include <var.h>

auto Fmt::Fmt(Db::Ent *x) -> std::string {
	std::stringstream ss;
	auto name = var_to_str(x->name);

	ss << name << ' ' << Db::EntTy_names[(S)x->ty] << ' ';
	switch (x->ty) {
	CASE(Db::Int, ss<<x->i)
	CASE(Db::Sz,  ss<<x->z)
	CASE(Db::Flt, ss<<x->f)
	CASE(Db::Dbl, ss<<x->d)
	CASE(Db::INT, ss<<Fmt::Fmt(&x->iA))
	CASE(Db::SZ,  ss<<Fmt::Fmt(&x->zA))
	CASE(Db::FLT, ss<<Fmt::Fmt(&x->fA))
	CASE(Db::DBL, ss<<Fmt::Fmt(&x->dA))
	}

	free(name);
	return ss.str();
}
