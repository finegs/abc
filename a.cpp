
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

int mhash(const char* str) {
	int h;
	h = 5381;
	char* s = (char*)str;
	while(*s++) { h=((h<<5)+h)+s[0]; h%= 1024; }
	return h;
}

int mmemset(void* p, char c, size_t len) {
	while(len--) {
		((char*)p+len-1)[0]=c;
	}
	return 0;
}

int main(int argc, char* argv[]) {
	AA aa{};
	aa.m = (0x01)|(0x02<<2)|(0x03<<6);
	cout << "size:" << sizeof(AA) << endl;
	cout << (int)aa.m1<<","<<(int)aa.m2<<","<<(int)aa.m3<<endl;

	char* str = (char*)malloc(sizeof(char)*1024);
	mmemset(str, 0, 1024);
	printf("Enter:"); fflush(stdout);
	scanf("%s", str); fflush(stdin);
	printf("mhash(%s)=%d\n", str, mhash(str));
	return 0;
}
