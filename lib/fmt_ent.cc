#include <sstream>

#include <db.h>

auto Fmt::Ent(Db::Ent *x) -> std::string {
	std::stringstream ss;

	ss << x->name << ' ' << Db::EntTy_names[(S)x->ty] << ' ';
	switch (x->ty) {
	CASE(Db::Int, ss<<x->i)
	CASE(Db::Sz,  ss<<x->z)
	CASE(Db::Flt, ss<<x->f)
	CASE(Db::Dbl, ss<<x->d)
	}

	return ss.str();
}
