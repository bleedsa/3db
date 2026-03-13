#include <filesystem>
#include <sstream>
#include <fstream>

#include <u.h>
#include <fs.h>
#include <clock.h>

using namespace std::filesystem;

constexpr auto success = "\e[32msuccess\e[0m";
constexpr auto failure = "\e[31mfail\e[0m";

int main() {
	FILE *P;
	Three::init();

	for (auto &d : directory_iterator{"o/run/"}) {
		auto p = d.path();
		if (p.extension() == ".com") {
			auto f = p.stem();
			std::stringstream ss;
			ss << "run/" << f.c_str() << ".com.txt";

			auto res = Fs::F_read_to_str(ss.str().c_str());
			if (!res) {
				std::cerr << res.error() << std::endl;
				exit(-1);
			}
			auto e = *res;

			std::string c;
			auto t = Clk::time([&]() {
				P = popen(p.c_str(), "r");
			});
			for (char i = fgetc(P); i != EOF; i = fgetc(P))
				c.push_back(i);
			pclose(P);

			std::cout << f.c_str() << "...";
			std::cout << (c==e ? success : failure);
			std::cout << " in " << t;
			std::cout << std::endl;

		}
	}

	Three::deinit();
	return 0;
}
