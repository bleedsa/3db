#ifndef __3DB_FS_H__
#define __3DB_FS_H__

#include <fstream>

#include <u.h>

namespace Fs {
	/* check if file exists */
	inl auto F_exists(const char *path) -> bool {
		if (FILE *f = fopen(path, "r")) {
			fclose(f);
			return true;
		} else {
			return false;
		}
	}

	R<std::string> F_read_to_str(const char *path);

	constexpr auto BIN_FLAGS =
		std::ios::binary
		| std::ios::in
		| std::ios::out
		;
	constexpr auto BIN_TRUNC = BIN_FLAGS|std::ios::trunc;

	/* a binary read stream */
	struct Bin {
		std::fstream f;

		inl Bin(const char *path, bool t = false)
			: f{std::fstream(path, t?BIN_TRUNC:BIN_FLAGS)}
		{}

		inl ~Bin() {}

	};

	/*
	 * writes
	 */
	inl auto operator<<(Bin &x, u64 &y) -> Bin& {
		x.f.write(reinterpret_cast<char*>(&y), Z(u64));
		return x;
	}

	inl auto operator<<(Bin &x, u8 &y) -> Bin& {
		x.f.write(reinterpret_cast<char*>(&y), Z(u8));
		return x;
	}

	/* 
	 * reads
	 */
	inl auto operator>>(Bin &x, u64 &y) -> Bin& {
		x.f.read(reinterpret_cast<char*>(&y), Z(u64));
		return x;
	}

	inl auto operator>>(Bin &x, u8 &y) -> Bin& {
		x.f.read(reinterpret_cast<char*>(&y), Z(u8));
		return x;
	}
}

#endif
