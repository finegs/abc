#include <iostream>
#include <stdio.h>

#include "hash.hpp"
#include "hash_map.hpp"

typedef unsigned int muint;

int main(int argc, char* argv[]) {

	for (int i = 1; i< argc; i++) {
		printf("%d : hash(%s)=%d\n", i, argv[i], hash((const void*)argv[i])); 
	}

	MyHashMap<muint, char*> *map = new MyHashMap<muint, char*>;
	for (int i = 1; i< argc; i++) {
		printf("%d : hash(%s)=%d\n", i, argv[i], hash((const char*)argv[i])); 
		map->Insert(hash((const void*)argv[i]), argv[i]);
	}

	map->Print();

	delete(map);

	return 0;
}
