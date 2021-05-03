#include <stdio.h>
#include <malloc.h>
#include "MyHashMap.hpp"

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
		explicit MString(size_t capacity = 8193) : str(nullptr), len(0), capacity(capacity) {

		}	
		explicit MString(const char* str) {
			len = strlen(str);
			capacity = len+1;
			str = new char[capacity];
			memset(MString::str, '\0', capacity);
			strncpy(MString::str, str, len);
		}

		friend std::ostream& operator<<(std::ostream& os, const MString& o) {
			os << "len=" <<o.len << ", capacity="<<o.capacity<<", str="<<o.str;
			return os;
		}

		friend std::istream& operator>>(std::istream& is, MString& o) {
			memset(o.str, '\0', o.capacity);
			is >> o.str;
			o.len = strlen(o.str);
#if 0

			if(!o.str) {
				o.len = 0;
				o.capacity = 8193;
				o.str = new char[o.capacity];
				memset(o.str, '\0', o.capacity);
			}
			else {
				o.len = 0;
				memset(o.str, '\0', strlen(o.str));
			}

			//while(is.get(c) || c != ' ' || c != '\n' | c != '\t') {
			size_t buf_len = 256;
			char buf[buf_len];
			memset(buf, '\0', buf_len);

			// TODO : read buf until std::istream.get is OK
			size_t i = 0;
			while(is.get(buf, buf_len)) {
				if(i>=o.capacity) {
					o.capacity *= 2;
					o.str = (char*)realloc(o.str, o.capacity);
				}
				strncpy(o.str+i, buf, buf_len); 
				o.len += buf_len;
				i+=buf_len;
			}
			o.str[i] = '\0';
			o.len = i;
#endif
			return is;
		}

		const char* getStr() const { return str; }

	private:
		char* str;
		size_t len;
		size_t capacity;
};

struct MStringHash {
	unsigned long operator() (const MString& o) const {
		return MyCharsKeyHash(o.getStr());
	}
};

struct MStringViewer {
	void operator()(FILE* fp, const MyHashNode<MString, MString>& o) const {
		fprintf(fp, "{Key:%s, Value:%s}", o.getKey(), o.getKey());
	}
};


int main() {

    MyHashMap<MString, MString, MStringViewer, MStringHash, 1000> map;


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
