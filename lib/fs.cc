#include <sstream>

#include <fs.h>

auto Fs::F_read_to_str(const char *path) -> R<std::string> {
	std::stringstream ss;
	std::ifstream in;

	if (!F_exists(path)) return err_fmt("file does not exist: {}", path);

	in.open(path);
	ss << in.rdbuf();

	return ss.str();
}
