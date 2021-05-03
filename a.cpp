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
