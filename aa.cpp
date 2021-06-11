#include <cstdio>
#include <malloc.h>
#include <type_traits>
//#include <utility>
#include "MyHashMap.hpp"


#if 0
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

int strncpy(char* dst, const char* src, size_t len) {
	if(!dst) return -1;
	char* sp = (char*)src;
	while(len-->0) *dst++ = *sp++; 
	return 0;
}

size_t strlen(const char* str) {
	size_t len = 0;
	char* c = (char*)str;
	while(*c++) len++;
	return len;
}

#endif

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
		explicit MString(size_t capacity = 8193) 
			: str(nullptr), len(0), capacity(capacity) {
		}	

		explicit MString(const char* str) {
			len = strlen(str);
			capacity = len+1;
			this->str = new char[capacity];
			memset(this->str, '\0', capacity);
			strncpy(this->str, str, len);
		}

		// copy construct
		MString(const MString& o) {
			clone(o);
		}

		// copy operator=
		const MString& operator=(const MString& o) {
			if(this == &o) return o;
			dispose();
			clone(o);
			return *this;
		}

		// move construct
		MString(MString&& o) {
			move(std::forward<MString&&>(o));
		}

		const MString& operator=(MString&& o) {
			if(this==&o) return o;
			dispose();
			move(std::forward<MString&&>(o));
			return *this;
		}

		void dispose() {
			if(!str) return;
			delete[] str;
			str = nullptr;
			len = capacity = 0;
		}

		void clone(const MString& o) {
			len = strlen(o.str);
			capacity = len+1;
			str = new char[capacity];
			strncpy(str, o.str, len);
			str[len] = '\0';
		}

		void move(MString&& o) {
			str = std::move(o.str);
			len  = o.len;
			capacity = o.capacity;

			o.str = nullptr;
			o.len = o.capacity = 0;
		}

		bool operator==(const MString& o) const {
			return !strcmp(str, o.str);
		}

		bool operator!=(const MString& o) const {
			return strcmp(str, o.str);
		}

		friend std::ostream& operator<<(std::ostream& os, const MString& o) {
			os << "{len=" <<o.len << ", capacity="<<o.capacity<<", str="<<o.str << "}";
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
		return strhash(o.getStr(), 1024*1024*1024);
	}
};

struct MStringViewer {
	void operator()(FILE* fp, const MyHashNode<MString, MString>& o) const {
		fprintf(fp, "{Key:%s, Value:%s}", o.getKey().getStr(), o.getValue().getStr());
	}
};

template<typename t, size_t n>
inline void array_copy(t (&dst)[n], t (&src)[n]) {
	static_assert(std::is_trivially_copy_assignable<t>::value, "element type has non-trivial copy assignment");
	memcpy(dst, src, n*sizeof(t));
#if 0
	for (size_t i = 0; i < n; ++i) {
		dst[i] = src[i];	
	}
#endif
}

void do_test_t2();

bool g_run;

int main() {
	using namespace std;

    MyHashMap<MString, MString, MStringViewer, MStringHash, 1000> map;

	size_t cmd_len = 1024+1, key_len = 1024+1, value_len = 1024+1, exec_cnt = 0;
	char cmd[cmd_len];
	g_run = true;
	while(g_run) {
		memset(cmd, '\0', cmd_len);
		cout << "[" << ++exec_cnt << "] Enter (--add key value, --get key, --remove key, --print, --clear, --exit) : "; cout.flush();
		cin>>cmd;
		if(!strcmp(cmd, "--add") || !strcmp(cmd, "-a")) {
			char key[key_len], value[value_len];
			memset(key, '\0', key_len);
			memset(value, '\0', value_len);
			cin >> key >> value;
			MString mk(key), mv(value);
			map.put(mk, mv);
        }
		else if(!strcmp(cmd, "--get") || !strcmp(cmd, "-g")) {
			char key[key_len];
			memset(key, '\0', key_len);
			cin >> key;
			MString ms(key);
			MString mm;
			if(map.get(ms, mm)) {
				cout << "Found : key="<< ms << ", value=" << mm << std::endl;
			}
			else {
				cout << "Not found : " << key << std::endl;
			}
		}
		else if(!strcmp(cmd, "--remove") || !strcmp(cmd, "-r")) {
			char key[key_len];
			memset(key, '\0', key_len);
			cin >> key;
			MString ms(key);
			map.remove(ms);
		}
		else if(!strcmp(cmd, "--print") || !strcmp(cmd, "-p")) {
			map.print(stdout);
		}
		else if(!strcmp(cmd, "--clear") || !strcmp(cmd, "-c")) {
			map.clear();
		}
		else if(!strcmp(cmd, "--exit") || !strcmp(cmd, "-e") || !strcmp(cmd, "-q")) {
			g_run = false;
		}
		else if(!strcmp(cmd, "--t2") || !strcmp(cmd, "-t2")) {
			do_test_t2();
		}
		else {
			cout << "Unsupported Cmd : " << cmd << std::endl;
		}
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

void do_test_t2() {
	int a[3] = {1,2,3}, b[3]={0};

	array_copy<int, 3>(b, a);

	for (int i = 0; i < 3; ++i) {
		printf("[%d]=%d\t", i, b[i]);
	}
	printf("\n");

}
