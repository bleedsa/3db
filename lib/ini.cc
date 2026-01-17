#include <cctype>
#include <sstream>

#include <str.h>
#include <fs.h>
#include <ini.h>

enum ParseState {
	DEFAULT,
	KEY,
	EQUALS,
	VALUE,
	ENDL,
};

auto INI::INI::parse(const char *path) -> char* {
	char c, *err;
	ParseState state;
	std::stringstream ss;
	std::string key, val;

	/* read the file */
	auto fres = Fs::F_read_to_str(path);
	if (!fres) return A_err("failed to parse ini file: {}", fres.error());
	auto cont = *fres;
	auto src = cont.c_str();

	/* internal parser functions */
	auto inc = [&]() {c = *++src;};
	#define err_prs(f...) {err = A_err(f);goto panic;}

	/* the main parser loop */
	for (c = *src, state = DEFAULT; c;) {
		switch (state) {
		/* default behavior is to check for a key */
		case DEFAULT:
			if (c != '=')  {
				/* setup for a key */
				state = KEY;
				ss = std::stringstream();
			} else {
				err_prs("expected key, got {}", c);
			}
			break;

		/* parse out a key into the key variable */
		case KEY:
			if (c != '=') {
				ss << c;
				inc();
			} else {
				state = EQUALS;
				key = ss.str();
				ss = std::stringstream();
			}
			break;

		/* inc past the equals sign */
		case EQUALS:
			if (c == '=') {
				inc();
				state = VALUE;
			} else {
				err_prs("expected '=', got {}", c);
			}
			break;

		/* parse out a value into the val variable */
		case VALUE:
			if (c != '\n' && c != EOF) {
				ss << c;
				inc();
			} else {
				state = ENDL;
				val = ss.str();
				ss = std::stringstream();
			}
			break;

		/* add the value to the map */
		case ENDL:
			map[key] = val;
			inc();
			state = DEFAULT;
			break;

		default: return A_err("unknown state {}", (S)state);
		}
	}

	return nullptr;

panic:
	return err;
}
