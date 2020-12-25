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


int main(int argc, char* argv[]) {
	AA aa{};
	aa.m = (0x01)|(0x02<<2)|(0x03<<6);
	cout << "size:" << sizeof(AA) << endl;
	cout << (int)aa.m1<<","<<(int)aa.m2<<","<<(int)aa.m3<<endl;
	return 0;
}
