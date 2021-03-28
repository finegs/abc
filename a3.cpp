#include <iostream>

#include <vector>
#include <algorithm>

using namespace std;

auto find_missing_element(const std::vector<uint64_t>& v) {
	const auto it = std::adjacent_find(std::cbegin(v), std::cend(v),
			[](auto x, auto y) { return y-x>1; });
	return *it+1;
}

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
	vector<uint64_t> a={1,2,4};

	cout<< find_missing_element(a) << endl;

	constexpr double result = circle_area<radius::value>::value;
	static_assert(result==19.625, "not equal");
	std::cout << result << std::endl;
	return 0;
}
