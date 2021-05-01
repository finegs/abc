#include <stdio.h>
#include <malloc.h>
//#include <string.h>
#include "List.h"
#include "Util.h"
#include "hash_map.hpp"
#include <memory>

int memset(void* p, char c, size_t len) {
	char* pp = (char*)p;
	while(len--) {
		*pp++ = c;
	}
	return 0;
}

int strcmp(const char* a, const char* b) {
	char* aa = (char*)a;
	char* bb = (char*)b;
	while(*aa) {
		if(*aa != *bb) break;
		aa++;
		bb++;
	}
	return *(unsigned char*)aa - *(unsigned char*)bb;
}

void strncpy(char* dst, const char* src, size_t len) {
	char* sp = (char*)src;
	while(len--) *dst++ = *sp++; 
}

size_t strlen(const char* str) {
	size_t len = 0;
	char* c = (char*)str;
	while(*c++) len++;
	return len;
}

struct MyCharsIntViewer {
  void operator()(const char *key, int value) const {
    printf("(%s, %d)", key, value);
  }
};

struct MyCharsMatcher {
	int operator()(const char* str1, const char* str2) const {
		return strcmp(str1, str2);
	}
};
struct MyCharsHash {
	unsigned long operator()(const char* str) {
		return strhash(str);
#if 0
		unsigned long h = 5417;
		char* c = (char*)str;
		while(*c) {
			h = h<<4 | h;
			h += *c;
			c++;
		}
		return h;
#endif
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

class MString {
	public:
		explicit MString(size_t capacity = 8193) : str(nullptr), len(0), capacity(capacity) {}	
		explicit MString(const char* str) : capacity(8193) {
			str = new char[capacity];
			memset(MString::str, '\0', capacity);
			strncpy(MString::str, str, len = strlen(str));
		}

		friend std::ostream& operator<<(std::ostream& os, const MString& o) {
			os << "len=" <<o.len << ", capacity="<<o.capacity<<", str="<<o.str;
			return os;
		}

		friend std::istream& operator>>(std::istream& is, MString& o) {
			size_t buf_len = 8193;
			size_t b_len = buf_len;
			char c;

			int i = o.len;
			if(!o.str) {
				o.str = new char[buf_len];
				memset(o.str, '\0', buf_len);
			}

			//while(is.get(c) || c != ' ' || c != '\n' | c != '\t') {
			while(is.get(o.str, buf_len)) {
				if(i>=b_len) {
					b_len *= 2;
					o.str = (char*)realloc(o.str, b_len);
				}
				o.str[i++]=c;
			}
			o.str[i] = '\0';
			o.len = i;
			o.capacity = b_len;
			return is;
		}

	private:
		char* str;
		size_t len;
		size_t capacity;
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

    HashMap<const char *, int, MyCharsIntViewer, MyCharsHash, MyCharsMatcher> map;

    size_t str_len = 1024+1;
	char cmd[32+1], str[str_len], value[12];

	while(1) {
		memset(str, 0, str_len);
		memset(cmd, 0, 32+1);
		memset(value,0,12);

    	printf("Enter cmd [key value] : "); fflush(stdout);
    	scanf("%s", cmd);
		if(!strcmp("--exit", cmd) || !strcmp("--quit", cmd)) break;
		else if (!strcmp("--help", cmd) || !strcmp("-h", cmd)) { printUsage(); }
		else if (!strcmp("--add",cmd) ||!strcmp("-a", cmd)) { 
			scanf("%s %s", str, value); 
			printf("[Info] %-10s : (%s, %s)\n", cmd, str, value); 
			map.put(str, atoi(value)); 
			printf("[Info] %-10s : completed(size=%lld) (%s, %s)\n", cmd, map.getSize(), str, value); 
		}
		else if (!strcmp("--remove",cmd) ||!strcmp("-r", cmd)) { 
			scanf("%s", str); 
			printf("[Info] %-10s : (%s)\n", cmd, str); 
			map.remove(str); 
		}
		else if (!strcmp("--clearall",cmd) ||!strcmp("-ca", cmd)) { 
			printf("[Info] %-10s : Are your sure to cleanAll? (yes/No)\n", cmd); 
			scanf("%s", str);
			if(!strcmp("y",str) || !strcmp("Y",str) || !strcmp("yes",str) || !strcmp("YES", str)) {
				map.clearall(); 
				printf("[Info] %-10s : CleanAll is done.\n", cmd); 
			}
		}
		else if (!strcmp("--get",cmd) ||!strcmp("-g", cmd)) { 
			int value = 0 ;
			scanf("%s", str); 
			bool rb = map.get(str, value); 
			printf("[Info] %-10s : (%s= get(%s, %d)\n", cmd, (rb? "true" : "false"), str, value);

		}
		else if (!strcmp("--print",cmd) ||!strcmp("-p", cmd)) { 
			map.print(); 
		}
		else if (!strcmp("--test01", cmd)) {
			std::unique_ptr<MString> p1 = std::make_unique<MString>(1025);
			std::cout << "Enter String to test MString :"; std::cout.flush();
			std::cin >> *p1.get();

			std::cout << "MString : " << *p1.get() << std::endl;
		}
		else if (!strcmp("--test02", cmd)) {
			char buf[100];
			memset(buf, 0, 100);

			std::cin.clear();
			std::cout.flush();
			std::cout<<"Enter Message : "; std::cout.flush();
			std::cin.get(buf, 100);
			std::cout << std::endl << "Your Message : " << buf << std::endl;
 		}
		else { printUsage(); }

		char* ts = str;
		while(*ts == '-') { ts++; }
    	printf("\t\t\t[Debug] hash(%s) : %d\n", ts, strhash(ts));
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
