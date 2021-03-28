#include <iostream>

#include <vector>
#include <algorithm>

using namespace std;

auto find_missing_element(const std::vector<uint64_t>& v) {
	const auto it = std::adjacent_find(std::cbegin(v), std::cend(v),
			[](auto x, auto y) { return y-x>1; });
	return *it+1;
}

int main() {

	vector<uint64_t> a={1,2,4};

	cout<< find_missing_element(a) << endl;

	return 0;
}
