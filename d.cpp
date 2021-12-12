#include <ratio>
#if 1
#include <iostream>
#include <vector>
#include <cstring>
#include <chrono>
#include <sys/timeb.h>
#include <ctime>
#include <cassert>


using std::chrono::system_clock;

class Item {
	private:
	int a;
	char* b;
	public:
	Item() : a{0}, b{nullptr} {}
	Item(int a_, const char* b_) : a{a_}, b{nullptr} {
		b = new char[strlen(b_)+1]{'\0'};
		strcpy_s(b, strlen(b_), b_);
	}
	const int getA() const { return a; }
	const char* getB() const { return b; }
};

int sum(std::vector<Item>& list) {
	int sum = 0;
	for(auto& i : list) { sum += i.getA(); }
	return sum;
}
int sum2(std::vector<Item>const & list) {
	int sum = 0;
	for(auto& i : list) { sum += i.getA(); }
	return sum;
}

size_t elapsedmsec(const std::chrono::time_point<std::chrono::system_clock>& start) {
	auto end = std::chrono::system_clock::now();
	return std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count();
}

char* timestamp(char *str) {
	auto currentTime = system_clock::now();

	auto transformed = currentTime.time_since_epoch().count() / 1000000;

	auto millis = transformed % 1000;

	std::time_t tt = system_clock::to_time_t ( currentTime );
	struct tm newtime;
	errno_t rc = localtime_s(&newtime, &tt);
	assert(ENOMSG != rc);
	strftime (str, 25,"%Y-%m-%d %H:%M:%S", &newtime);
	sprintf(str, "%s.%03d", str,(int)millis);
	return str;
}


int main() {
	char tmstr[25]{'\0'};
	size_t diff{0}, summ{0}, avg{0};
	size_t cc = 10;

	std::vector<Item> list{{1, "aa"}, {2, "bb"}};
	std::cout << timestamp(tmstr) << "#1 Start" << std::endl;
	for (int ii = 0; ii < cc; ++ii) {
		auto start = std::chrono::system_clock::now();
		for (int i = 0; i < 10000000; ++i) {
			sum(list);
		}
		diff = elapsedmsec(start);
		summ+=diff;
	}
	std::cout << timestamp(tmstr) << "#1 End : " << ", sum="<< summ << ", avg="<< (summ/cc) << diff << std::endl;


	summ = avg = diff = 0;
	std::cout << timestamp(tmstr) << "#2 Start" << std::endl;
	for (int ii = 0; ii < cc; ++ii) {
		auto start = std::chrono::system_clock::now();
		for (int i = 0; i < 10000000; ++i) {
			sum2(list);
		}
		diff = elapsedmsec(start);
		summ+=diff;
	}
	std::cout << timestamp(tmstr) << "#2 End : " << ", sum="<< summ << ", avg="<< (summ/cc) << diff << std::endl;

};

#endif
#if 0

#include <stdio.h>
#include <malloc.h>

char* mstrcpy(char dst[], const char src[]) {
    int i = 0;
	char* rt = dst;
    while((dst[i] = src[i]) != '\0') ++i; 
    return rt;
}

unsigned long mstrcmp(const char str1[], const char str2[]) {
    int i = 0;
    while(str1[i] != '\0' && str1[i] == str2[i]) ++i;
    return str1[i] - str2[i];
}

unsigned int mstrhash(const char str[], unsigned int TABLE_SIZE) {
    unsigned int h = 5381;
    char c = 0;
    while((c = *str) != '\0') {
        h = (((h<<5) + h) + c) % TABLE_SIZE;
		++str;
    }
    return h % TABLE_SIZE;
}

int main(int argc, char* argv[]) {
//    char* src = (char*)malloc(256);
    char* src = new char[256+1]{0};
    if(argc > 1)
		mstrcpy(src, argv[1]);
    else 
        mstrcpy(src, "abc");
    
    char dst[256]{0};
    mstrcpy(dst, src);
    printf("mstrcmp(%s, %s=mstrcpy(%s))=%s\n", 
			src, dst, src, (mstrcmp(src, dst) == 0 ? "true" : "false"));
    printf("mstrhash(%s)=%u\n", src, mstrhash(src, 1000000));

    delete[] src;
	
    return 0;
}

#endif
