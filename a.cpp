#include <iostream>
#include <stdio.h>

#include "Util.h"
#include "hash_map.hpp"

typedef unsigned long mulong;

int main(int argc, char* argv[]) {

	for (int i = 1; i< argc; i++) {
		printf("%d : hash(%s)=%zu\n", i, argv[i], strhash(argv[i], 1024*1024*1024)); 
	}

	HashMap<mulong, char*> *map = new HashMap<mulong, char*>;
	for (int i = 1; i< argc; i++) {
		printf("%d : hash(%s)=%zu\n", i, argv[i], strhash(argv[i], 1024*1024*1024)); 
		map->put(strhash(argv[i], 1024*1024*1024), argv[i]);
	}

	map->print();

	delete(map);

	return 0;
}
