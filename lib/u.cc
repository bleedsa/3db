#include <u.h>
#include <str.h>
#include <db.h>

void Three::init() {
	Str::Interns::init();
}

void Three::deinit() {
	Str::Interns::deinit();
	Db::deinit();
}
