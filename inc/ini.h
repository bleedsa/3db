#ifndef __3DB_INI_H__
#define __3DB_INI_H__

#include <unordered_map>

#include <u.h>

namespace INI {
	struct INI {
		std::unordered_map<std::string, std::string> map;

		inl INI()
			: map{std::unordered_map<std::string, std::string>()}
		{}

		inl ~INI() {}

		char *parse(const char *path);

		inl INI(const char *path)
			: map{std::unordered_map<std::string, std::string>()}
		{
			char *err;
			if ((err = parse(path))) {
				throw err;
			}
		}
	};
}

#endif
