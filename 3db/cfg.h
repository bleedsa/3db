#ifndef __3DB_3DB_CFG_H__
#define __3DB_3DB_CFG_H__

struct Cfg {
	std::string file, port;

	Cfg(const char *path);
	inl ~Cfg() {}
};

#endif
