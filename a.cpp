#include <iostream>
#include <stdio.h>

#include "Util.h"
#include "hash_map.hpp"
#include "array.hpp"


typedef unsigned int muint;

struct UIntHasher  {
	unsigned long operator()(const muint& key) const {
		return key;
	}
};

struct UIntStrViewer {
	void operator()(const muint& key, char* value) const {
		printf("{%d, %s}", key, value);
	}
};

struct UIntMatcher {
	bool operator()(const muint& a, const muint& b) const {
		return a == b;
	}
};

void do_test_array();

int main(int argc, char* argv[]) {
	int tc = 0;

	printf("#%d : Input\n", ++tc);
	for (int i = 1; i< argc; i++) {
		printf("%d : hash(%s)=%zu\n", i, argv[i], strhash(argv[i], 1024*1024*1024)); 
	}

	HashMap<muint, char*, UIntStrViewer, UIntHasher, UIntMatcher> *map 
		= new HashMap<muint, char*, UIntStrViewer, UIntHasher, UIntMatcher>;

	printf("#%d : Put\n", ++tc);
	for (int i = 1; i< argc; i++) {
		printf("%d : hash(%s)=%zu\n", i, argv[i], strhash(argv[i], 1024*1024*1024)); 
		map->put(strhash(argv[i], 1024*1024*1024), argv[i]);
	}

	printf("#%d : Print\n", ++tc);
	map->print();

	printf("#%d : Remove\n", ++tc);
	for (int i = 1; i< argc; i++) {
		map->remove(strhash(argv[i], 1024*1024*1024));
	}

	delete(map);


	do_test_array();

	return 0;
}

void do_test_array() {

	int arr[3] = {1, 2,100};

	Array<int, 3> arr2(arr);

	arr2.print();

}
