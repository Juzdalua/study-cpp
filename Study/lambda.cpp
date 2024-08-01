#include <iostream>

void study_lambda(){
    int a = 10;

	auto x = [num = 1](int b)
	{ return num + b; };

	int numbers = 2;
	auto y = [&numbers](int b){return numbers + b;};

	int c = x(a);
	std::cout << c << std::endl;
	
	int d = y(a);
	std::cout << d << std::endl;
}