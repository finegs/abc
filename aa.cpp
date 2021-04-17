#include <stdio.h>
#include <malloc.h>
//#include <string.h>
#include "List.h"
#include "Util.h"
#include "hash_map.hpp"

int memset(void* p, char v, size_t len) {
	char* c = (char*)p;
	while(len--) {
		*c = v;
		c++;
	}
	return 0;
}
int strcmp(const char* a, const char* b) {
	char* aa = (char*)a;
	char* bb = (char*)b;
	while(1) {
		if(aa[0] != bb[0]) return (aa[0] > bb[0] ? 1 : -1);
		if(!*aa) break;
		aa++;b++;
	}
	return 0;
}

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

void printUsage() {
	printf("Usage : cmd [ key value]\n");
	printf("        --exit, --quit\n");
	printf("        --add(-a) key value\n");
	printf("        --remove(-r) key\n");
	printf("        --get(-g) key\n");
	printf("        --print(-p)\n");
	printf("        --clearall(-ca)\n");
}


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

	HashMap<const char*, int, MyCharsHash> map;

	size_t str_len = 1024+1;
	char cmd[32+1], str[str_len], value[12];

	while(1) {
		memset(str, 0, str_len);
		memset(cmd, 0, 32+1);
		memset(value,0,12);

    	printf("Enter cmd [key value] : "); fflush(stdout);
    	scanf("%s", cmd);
		if(!strcmp("--exit", cmd) || !strcmp("--quit", cmd)) break;
		else if(!strcmp("--help", cmd) || !strcmp("-h", cmd)) { printUsage(); }
		else if(!strcmp("--add",cmd) ||!strcmp("-a", cmd)) { scanf("%s %s", str, value);  map.put(str, atoi(value)); }
		else if (!strcmp("--remove",cmd) ||!strcmp("-r", cmd)) { scanf("%s", str); map.remove(str); }
		else if (!strcmp("--clearall",cmd) ||!strcmp("-ca", cmd)) { map.clearall(); }
		else if (!strcmp("--get",cmd) ||!strcmp("-g", cmd)) { scanf("%s", str); map.remove(str); }
		else if (!strcmp("--print",cmd) ||!strcmp("-p", cmd)) { map.print(); }
		else { printUsage(); }
		
    	printf("\t\t\t[Debug] hash(%s) : %d\n", cmd, strhash(cmd));
		fflush(stdin);
	}

#if 0
	int n;
	printf("number : "); fflush(stdout);
	scanf("%d", &n); fflush(stdin);
	printf("prime_max(%d) : %d\n", n, prime_max(n));
#endif

#if 0
	HashMap<const char*, int, MyCharsHash> map;
	map.put("1", 1);
	int iv;
	bool b = map.get("1", iv);
	std::cout<< "map.get("<< "1" << ") = " << (b ? "true" : "false") << ", value = " << iv << std::endl;
#endif

	return 0;
}
