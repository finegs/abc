#include <iostream>

struct pi {
	static constexpr double value = 3.14;
};

template <const double& r>
struct circle_area{
	static constexpr double value = r*r*pi::value;
};


struct radius {
	static constexpr double value = 2.5;
};

int main() {
	constexpr double result = circle_area<radius::value>::value;
	static_assert(result==19.625, "not equal");
	std::cout << result << std::endl;
	return 0;
}
