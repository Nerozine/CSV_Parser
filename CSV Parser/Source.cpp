#include <iostream>
#include <tuple>
#include "WriteTuple.h"


int main() {
	std::tuple <int, char, double, float, short> a(5, 'a', 3.2, 45.2, 4);

	std::cout << a << std::endl;
	std::cout << a;

	std::tuple<> G;

	std::cout << G << std::endl;


	return 0;
}