#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include "List.h"
#include "Util.h"
#include "hash_map.hpp"

struct MyCharsHash {
	unsigned long operator()(const char* str) {
		unsigned long h = 5417;
		char* c = (char*)str;
		while(*c) {
			h = h<<4 | h;
			h += *c;
			c++;
		}
		return h;
	}
};


int main() {
	List list;	

	list.insert(10);
	list.insert(20);
	list.insert(30);
	list.insert(40);
	list.insert(50);
	list.insert(56);

//	int a;
//	list.remove(&a);

	list.printList();

	size_t str_len = 1024+1;
	char str[str_len];
	memset(str, 0, str_len);

	while(1) {
    	printf("Enter string to hash : "); fflush(stdout);
    	scanf("%s", str); fflush(stdin);
		if(!strcmp("exit", str) || !strcmp("quit", str)) break;
    	printf("hash(%s) : %d\n", str, strhash(str));
	}

	int n;
	printf("number : "); fflush(stdout);
	scanf("%d", &n); fflush(stdin);
	printf("prime_max(%d) : %d\n", n, prime_max(n));


	HashMap<const char*, int, MyCharsHash> map;
	map.put("1", 1);
	int iv;
	bool b = map.get("1", iv);
	std::cout<< "map.get("<< "1" << ") = " << (b ? "true" : "false") << ", value = " << iv << std::endl;

	return 0;
}
