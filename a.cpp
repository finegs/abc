#if 0
#include <iostream>
#include <stdio.h>

#include "Util.h"
#include "hash_map.hpp"

typedef unsigned long mulong;

int main(int argc, char* argv[]) {

	for (int i = 1; i< argc; i++) {
		printf("%d : hash(%s)=%d\n", i, argv[i], hash((const void*)argv[i])); 
	}

	HashMap<mulong, char*> *map = new HashMap<mulong, char*>;
	for (int i = 1; i< argc; i++) {
		printf("%d : hash(%s)=%d\n", i, argv[i], hash((const char*)argv[i])); 
		map->put(hash((const void*)argv[i]), argv[i]);
	}

	map->print();

	delete(map);

	return 0;
}
#endif

#include <iostream>
#include <unordered_map>

using namespace std;

struct Location {
	int row;
	int col;

	bool operator==(const Location& o) const noexcept {
		return row == o.row && col == o.col;
	}

	std::size_t hash() const noexcept {
		return (row<1) ^ col;
	}
};

namespace std 
{
	template<>
	struct hash<Location> {
		std::size_t operator()(const Location& o) const noexcept {
			return (o.row < 1) ^ o.col;
		}
	};
}

int main() {
	unordered_map<Location, const char*> m;
	m.insert({{1,1}, "1_1"});
	m.insert({{1,3}, "1_1"});
	m.insert({{1,2}, "1_1"});
	m.insert({{5,1}, "1_1"});

	cout << "m.at[{" << 1 << "," << 1 << "}]=" << m.at({1,1}) << std::endl;
	cout << "m[{" << 1 << "," << 2 << "}]=" << m[{1,2}] << std::endl;

	for(auto i : m) {
		//cout << "m.at[{" << i.first.row << ", " << i.first.col << "}]=" << m.at({i.first.row, i.first.col}) << std::endl;
		cout << "m.at[{" << i.first.row << ", " << i.first.col << "}]=" << m.at({i.first.row, i.first.col}) << std::endl;
	}

}