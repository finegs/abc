#include <iostream>
#include <stdio.h>

#include "hash.hpp"

int main(int argc, char* argv[]) {
	for (int i = 1; i< argc; i++) {
		printf("%d : hash(%s)=%d\n", i, argv[i], hash(argv[i])); 
	}

	return 0;
}
