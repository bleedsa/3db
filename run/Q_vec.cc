#include <Q.h>

int main() {
	Q::Q q;

	q = Q::Q(1.2345);
	std::cout << Fmt::Fmt(&q) << std::endl;
	q.~Q();

	q = Q::Q(A::A<i32>{1, 2, 3});
	std::cout << Fmt::Fmt(&q) << std::endl;
	q.~Q();

	q = Q::Q(A::A<f64>{1.0, 2.0, 3.0});
	std::cout << Fmt::Fmt(&q) << std::endl;
	q.~Q();

	q = Q::Q(A::A<Chr>{'a', 'b', 'c'});
	std::cout << Fmt::Fmt(&q) << std::endl;

	return 0;
}
