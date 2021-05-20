
#include <algorithm>
#include <iostream>
#include "Util.h"

#include <random>
#include <ctime>
#include <sys/time.h>
#if defined(WIN32) || defined(_WIN64)
#include <Windows.h>
#else
#include <sys/time.h>
#endif
#include "a1.hpp"

using namespace	std;

#define swap(a,b) { int t = (a); (a) = (b); (b) = t; }

const char* timestamp();

void heap_push(int arr[], int pos) {
	if(pos<=1||arr[pos/2]>arr[pos]) return;
	swap(arr[pos/2], arr[pos]);
	heap_push(arr, pos/2);
}

void heap_pop(int arr[], int n, int pos) {
	int np = pos*2;
	if(np<n&&arr[np]<arr[np+1]) np++;
	if(np>n||arr[pos]>arr[np]) return;
	swap(arr[pos], arr[np]);
	heap_pop(arr, n, np);
}

const char* timestamp() {
	time_t rawtime;
	struct tm* ti;

	rawtime = time(NULL);
	ti = localtime(&rawtime);

	timeval curtime;
	gettimeofday(&curtime, NULL);
	int milli= curtime.tv_usec / 1000;

	static char tmstmp_buf[25];
	memset(tmstmp_buf, 0, 25);
	strftime(tmstmp_buf, 20, "%Y-%m-%d %H:%M:%S", ti);

	sprintf(tmstmp_buf+19, ".%03d", milli);
	return tmstmp_buf;
}

using namespace std;

namespace ns_gg {
	struct Ast {
		float radius = 12;
	};
	void blast(Ast* ast, float force);
}

struct Target {
	ns_gg::Ast* ast;
	Target(ns_gg::Ast* ast) : ast{ast} {}
	operator ns_gg::Ast*() const { return ast; }
};

bool blast(Target target);
template<typename T> void blast(T* o, float force);

void play(ns_gg::Ast* ast) {
	blast(ast,100);
}

namespace ns_gg {
	void blast(ns_gg::Ast* ast, float f) {
		printf("ns::gg blast\n");
	}
}

struct AA{
  union {
    struct {
      unsigned char m1:2;
      unsigned char m2:4;
      unsigned char m3:2;
    };
    unsigned char m;
  };
};

typedef struct {
	int cnt;
	char name[32];
} BB;


int main(int argc, char* argv[]) {

#if 1
	size_t b_len = 10;
	BB* b = (BB*)malloc(sizeof(BB)* b_len);

	memset((void*)b, '\0', sizeof(BB)*b_len);

	char i_buf[32];

	memset(i_buf, '\0', 32);
	for (int idx = 0; idx < b_len; ++idx) {
		char* pn = b[idx].name;
		strncpy(pn, "name", sizeof("name"));
		itoa(idx, pn+strlen(pn), 10);	
		b[idx].cnt = idx; 
	}

	for (int idx = 0; idx < b_len; ++idx) {
		printf("[%d] %d, %s\n", idx, b[idx].cnt, b[idx].name);
	}

#endif

#if 0
	ns_gg::Ast* ast = new ns_gg::Ast();
	play(ast);
#endif

#if 0
	AA aa{};
	aa.m = (0x01)|(0x02<<2)|(0x03<<6);
	cout << "size:" << sizeof(AA) << endl;
	cout << (int)aa.m1<<","<<(int)aa.m2<<","<<(int)aa.m3<<endl;

	char* str = (char*)malloc(sizeof(char)*1024);
	mmemset(str, 0, 1024);
	printf("Enter:"); fflush(stdout);
	scanf("%s", str); fflush(stdin);
	printf("mhash(%s)=%d\n", str, mhash(str));
#endif 
	return 0;

}

int hash(const char* str) {

	int h;
	h = 5381;
	char* s = (char*)str;
	while(*s++) { h=((h<<5)+h)+s[0]; h%= 1024; }
	return h;
}

