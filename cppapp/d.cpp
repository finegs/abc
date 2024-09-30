
#include <ostream>
#if 1

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <unordered_map>
#include <cstring>
#include "u.hpp"
#include <algorithm>
#include <ranges>

std::ostream& operator<<(std::ostream& os, const char* o) {
	os << o;
	return os;
}

namespace my {
	const static char obj_sep_default = ',';
	static char obj_sep = obj_sep_default;

	template <typename It>
	inline std::ostream& display(std::ostream& os, It begin, It end) {
		char sep[3]{'\0', ' ', '\0'};
		for(auto& cur=begin;cur!=end;++cur) {
			os << sep << *cur; sep[0]=',';
		}
		return os;
	}
};

class Person {
	protected:
		std::string first;
		std::string last;
		int val;
	public:
	Person() = default;

	Person(const std::string& f, const std::string& l, int v) 
		: first{f}, last{l}, val{v} {}
	Person(const Person& o) = default;
	Person(Person&& o) noexcept = default;
	Person& operator=(const Person&) = default;
	Person& operator=(Person&&) noexcept = default;
//	Person(const Person& o)
//   		: first{o.first}, last{o.last}, val{o.val} 	{
//	}
//	Person(Person&& o) noexcept 
//		: first{std::move(o.first)}, last{std::move(o.last)}, val{o.val} {
//	}
//	Person& operator=(const Person& o) {
//		if(this==&o) return *this;
//		first = o.first;
//		last = o.last;
//		val = o.val;
//		return *this;
//	}
//	Person& operator=(Person&& o) {
//		if(this==&o) return *this;
//		first = std::move(o.first);
//		last = std::move(o.last);
//		val = o.val;
//		return *this;
//	}
	virtual ~Person() = default; 
	virtual void print(int n) const = 0;

	bool operator<(const Person& o) const {
		return first < o.first || last < o.last || val < o.val;
	}
	
	virtual std::ostream& print(std::ostream& os) const {
		// os << "{";
		#ifndef NDEBUG
 		os << "\"&\":"<< this << ", ";
		#endif
		os << '"' << "first" << '"' << ":" << '"' << first << '"';
		os << ", \"" << "last" << '"' << ":" << '"' << last << '"';
		os << ", \"" << "val" << '"' << ":" << val;
 		// os << "}";
		return os;
	}
 	friend std::ostream& operator<<(std::ostream& os, const Person& o) {
		 os << "{";
		 o.print(os);
		 os << "}";
		return os;
 	}
};
  
class Customer : public Person {
	protected:
		std::vector<int> data;

	public:
		Customer();
		~Customer();
		Customer(const std::string& f, const std::string& l, int v)
			: Person(f, l, v)	{
			data.push_back(v);
		}
		Customer(const Customer& o) 
			:Person(o)	{
		}
		Customer(Customer&& o) noexcept
			: Person(std::forward<Person>(o))	{
		}
		Customer& operator=(const Customer& o) {
			if(this==&o) return *this;
			Person::operator=(o);	
			return *this;
		}
		Customer& operator=(Customer&& o) noexcept {
			if(this==&o) return *this;
			Person::operator=(std::forward<Person>(o));
			return *this;
		}
		virtual std::ostream& print(std::ostream& os) const override {
			Person::print(os) << my::obj_sep_default;
			os << "\"data\":"; my::display(std::cout, data.begin(), data.end());
			return os;
		}
		void print(int n) const override;
};
Customer::Customer() : Person(){}
Customer::~Customer() = default;

void Customer::print(int n) const {
	char __tmstr[25]{'\0'};
	std::stringstream ss;

	ss << "{";
	#ifndef NDEBUG
	// if constexpr(NDEBUG) {
	ss << "&=" << this << ", ";
	// }
	#endif

	ss << "[";
	ss << first << ", " << last << ", " << val;
	ss << "]";
	ss << "}";
	DBG_LOG2(ss.str().c_str());
}

namespace std
{
	template<>
	struct hash<const char*>
	{
		size_t operator()(const char* s) const {
			return mstrhash(s);
		}
	};

	template<>
	struct equal_to<const char*>
	{
		bool operator()(const char* a, const char* b) const {
			return strncmp(a,b, strlen(a)) == 0;
		}
	};
};

using KeyCustomer = std::pair<const char*, Customer>;
std::ostream& operator<<(std::ostream& os, const KeyCustomer& o) {
	os << '{' << o.first << ':' << o.second << '}';
	return os;
}

int main(int argc, char* argv[]) {
	std::vector<Person*> v;
	std::unordered_map<const char*, Customer> m;

	//Student student{"First", "Last", 10};
	for (int i = 0; i < argc; i++) {
		Customer* c1 = new Customer{argv[i], "Last", i};
		m.insert({argv[i], *c1});
		v.push_back(std::move(c1));

		c1->print(i);
	}
	my::obj_sep = '\n';
	std::cout << "v : "; my::display(std::cout, v.begin(), v.end()) << std::endl;
	my::obj_sep = '\n';
	std::cout << "m : "; my::display(std::cout, m.begin(), m.end()) << std::endl;

#if ((defined(_MSVC_LANG) && _MSVC_LANG >= 201703L) || __cplusplus >= 201703L)
     //C++17 specific stuff here
	for (auto& o : v) { delete o; }
#elif ((defined(_MSVC_LANG) && _MSVC_LANG >= 202002L) || __cplusplus >= 202002L)
	 //C++20 specific stuff here
	std::for_each(v.begin(), v.end(), [](auto& o) { delete o;});
#elif ( __GNUC__ > 0 )
	std::for_each(v.begin(), v.end(), [](auto& o) { delete o;});
#endif

	// for(auto& c : v) { c->print(1); delete c; }

	getchar();

	std::vector<int> vec{1,2,3,4,5};
	my::display(std::cout, vec.begin(), vec.end());
}

#endif

#if 0

#include <iostream>
#include <string>
#include <vector>

struct State {};

class IAListener {
	public:
		IAListener() = default;
		virtual ~IAListener() = default;
		virtual void onA(State& mState) const = 0;
};

class A : public IAListener {
	public:
	A() = default;
	~A() = default;
	void onA(State& mState) const override;
};

//A::A() {}
//A::~A() {}
void A::onA(State& mState) const {
}

int main() {
	std::vector<IAListener> data;
	A a;
	data.push_back(std::move(a));
	std::cout << &a;
	return 0;
}

#endif

#if 0
#include <iostream>
#include <vector>
#include <cstring>
#include <chrono>
#include <sys/timeb.h>
#include <ctime>
#include <cassert>
#include <cmath>


using std::chrono::system_clock;

// added by SGK 20211228
#ifndef TIME_UTC
#define TIME_UTC time(NULL)
#endif

class Item2 {
	public:
		Item2() : name{'\0'}{}
		Item2(const char* name_) : name{'\0'} {
			strncpy_s(name, 1000000, name_, strlen(name_));
		}

		const char* getName() const { return name; }
		void setName(const char* name_) {
			strncpy_s(name, 1000000, name_, strlen(name_));
		}
		friend std::ostream& operator<<(std::ostream& os, const Item2& o) {
			os << "{";
			os << "\"name\"" << ":" << "\"" << o.name << "\"";
			os << "}";
			return os;
		}

	private:
	char name[1000000];
};

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

const char* tmstr(char str[25]) {

    //time_t tm;
    struct timespec ts;
    struct tm ti;

    //tm = time(NULL);
    timespec_get(&ts, TIME_UTC);
    //ti = localtime(&tm);
    localtime_s(&ti, &ts.tv_sec);
    
    strftime(str, 25, "%Y-%m-%d %H:%M:%S", &ti);
    sprintf(str, "%s.%03ld", str, lround(ts.tv_nsec/1e6));
	return str;
}

template<typename T>
std::ostream& operator<<(std::ostream& os, std::vector<T>& v) {
	bool first = true;
	os << "[";
	for(auto& o : v) { os << (first ? "" : ", ") << o; first = false; }
	os << "]";
	return os;
}


int main(int argc,char* argv[]) {
	char tmstr[25]{'\0'};
	size_t diff{0}, summ{0}, avg{0};
	size_t cc = 10;

	std::vector<Item> list;
	std::vector<Item2> list2;
	for (int i = 1; i < argc; ++i) {
		list.push_back({i, argv[i]});	
		for (int j = 0; j < 5300; ++j) {
			list2.push_back({argv[i]});
		}
	}

	std::cout << timestamp(tmstr) << "#1 Start" << std::endl;
	for (int ii = 0; ii < cc; ++ii) {
		auto start = std::chrono::system_clock::now();
		for (int i = 0; i < 10000000; ++i) {
			sum(list);
		}
		diff = elapsedmsec(start);
		summ+=diff;
	}
	std::cout << timestamp(tmstr) << "#1 End : " << ", sum="<< summ << ", avg="<< (summ/(double)cc) << diff << std::endl;

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
	std::cout << timestamp(tmstr) << "#2 End : " << ", sum="<< summ << ", avg=" << summ/(double)cc << diff << std::endl;

	std::cout << timestamp(tmstr) << list2 << "\n";

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
