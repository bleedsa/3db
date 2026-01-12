#include <Q.h>

int main() {
	auto q1 = Q::Q(1.2345);
	std::cout << Fmt::Fmt(&q1) << std::endl;

	auto q2 = Q::Q(A::A<i32>{1, 2, 3});
	std::cout << Fmt::Fmt(&q2) << std::endl;

	auto q3 = Q::Q(A::A<S>{1, 2, 3});
	std::cout << Fmt::Fmt(&q3) << std::endl;

	auto q4 = Q::Q(A::A<f32>{1.0f, 2.0f, 3.0f});
	std::cout << Fmt::Fmt(&q4) << std::endl;

	auto q5 = Q::Q(A::A<f64>{1.0, 2.0, 3.0});
	std::cout << Fmt::Fmt(&q5) << std::endl;

	return 0;
}
