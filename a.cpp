#include <iostream>
#include <stdio.h>

#include "hash.hpp"
#include "hash_map.hpp"

int main(int argc, char* argv[]) {

	for (int i = 1; i< argc; i++) {
		printf("%d : hash(%s)=%d\n", i, argv[i], hash(argv[i])); 
	}

	typedef unsigned int muint;
	MyHashMap<muint, const char*> *map = new MyHashMap<muint, const char*>;
	for (int i = 1; i< argc; i++) {
		printf("%d : hash(%s)=%d\n", i, argv[i], hash(argv[i])); 
		map->Insert(hash(argv[i]), argv[i]);
	}

	map->Print();

	delete(map);

	return 0;
}
