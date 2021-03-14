
#include <iostream>

using namespace std;

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

int mhash(const char* str);
int mmemset(void* p, char c, size_t len);
size_t mstrncpy(char* p, const char* s, size_t s_len);
size_t mstrlen(const char* p);

int main(int argc, char* argv[]) {

	size_t b_len = 10;
	BB* b = (BB*)malloc(sizeof(BB)* b_len);

	mmemset((void*)b, '\0', sizeof(BB)*b_len);

	b[0].cnt = 1;
	mstrncpy(b[0].name, "str_1", mstrlen("str_1"));

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

int mhash(const char* str) {

	int h;
	h = 5381;
	char* s = (char*)str;
	while(*s++) { h=((h<<5)+h)+s[0]; h%= 1024; }
	return h;
}

int mmemset(void* p, char c, size_t len) {
	char* pp = (char*)p;
	size_t ll = len;
	while(ll--) {
		*(pp+ll)=c;
	}
	return 0;
}

size_t mstrlen(const char* p) {
	size_t l = 0;
	while(p++) { l++; }
	return l;
}

size_t mstrncpy(char* p, const char* s, size_t s_len) {
	size_t ll = s_len;
	while(s_len-->0) { *(p+s_len-1) = *(s+s_len-1); }
	return ll;
}
