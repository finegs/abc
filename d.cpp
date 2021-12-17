#include <_timeval.h>
#if 1
#include <iostream>
#include <vector>
#include <fstream>
#include <cstring>
#include <ctime>
#include <sys/timeb.h>

const char* timestamp(char str[24]) {
	struct timeb tb;
	struct tm ti;
	ftime(&tb);
	localtime_s(&ti, &tb.time);
	strftime(str, 23, "%Y-%m-%d %H:%M:%S", &ti);
	sprintf(str, "%s.%03d", str, tb.millitm);
	return str;
}

template<typename T>
std::ostream& operator<<(std::ostream& os, std::vector<T> o) {
	bool first = true;
	time_t t;
	os << " :  size()=" << o.size() << ", values=[";
	for (const T& t : o) {
		if(!first) os << ", ";
		else first = false;
		os << t;
	}
	os << "]";
	return os;
}

template<typename T>
void swap(T &a, T &b) {
	T t;
	t = a;
	a = b;
	b = t;
}

template<typename T>
void quicksort(std::vector<T>& v, int left, int right) 
{
	if(left>=right) return;
	int i = left-1;
	int pvt = right;
	for (int j = left; j < right; ++j) {
		if(v[j]<=v[pvt]) {
			i++;
			swap(v[i], v[j]);
		}
	}
	i++;
	swap(v[i], v[pvt]);

	pvt = i;
	quicksort(v, left, pvt-1); 
	quicksort(v, pvt+1, right);
}

void rand2file(const char* file, int size) {
	std::ofstream ofs;
	ofs.open(file, std::ios::out|std::ios::trunc);
	srand(time(NULL));
	for (int i = 0; i < size; ++i) {
		ofs << (i>0?" " : "" ) << rand() % 10000;
	}
	ofs<<"\n";
	ofs.close();
}

template<typename T>
std::vector<T> file2vector(const char* file, std::vector<T> &v) 
{
	std::ifstream ifs;
	ifs.open(file, std::ios::in);
	int i;
	while(ifs>>i) {
		v.push_back(i);
	}
	ifs.close();
	return v;
}

template<typename T>
void vector2file(const char* file, const std::vector<T>& v) 
{
	std::ofstream ofs;
	ofs.open(file, std::ios::out|std::ios::trunc);
	for (int i = 0; i < v.size(); ++i) {
		ofs << (i>0?" ":"") << v[i];
	}
	ofs<<"\n";
	ofs.close();
}


int main(int argc, char* argv[])
{
	bool verbose = false;
	bool stdqucksort = false;
	int osize = 10;
	char ifile[128]{"input.txt"};
	char ofile[128]{"output.txt"};
	std::vector<int> v;
	char tstr[24]{0};

	std::cout << timestamp(tstr) << " " << (stdqucksort ? "stdqucksort" : "myquicksort" ) << "\n";

	for (int i = 0; i < argc; ++i) {
		if(!strcmp("-if", argv[i]) && i+1 < argc) {
			strcpy(ifile, argv[i+1]);	
			i++;
			continue;
		}  

		if(!strcmp("-of", argv[i]) && i+1 < argc) {
			strcpy(ofile, argv[i+1]);	
			i++;
			continue;
		}

		if(!strcmp("-n", argv[i]) && i+1 < argc) {
			osize = atoi(argv[i+1]);	
			i++;
			continue;
		}
		if(!strcmp("-v", argv[i]) 
				|| !strcmp("--verbose", argv[i])) {
			verbose = true;
		}
		if(!strcmp("-stdquicksort", argv[i])) {
			stdqucksort = true;
		}
	}

	std::cout << timestamp(tstr) << " Before rand2file" << "\n";
	rand2file(ifile, osize);

	std::cout << timestamp(tstr) << " After rand2file" << "\n";
	v.clear();
	std::cout << timestamp(tstr) << " Before file2vector" << "\n";
	file2vector<int>(ifile, v);

	std::cout << timestamp(tstr) << " After file2vector" << "\n";
//	std::vector<int> v; v.reserve(argc-1);	
//	for (int i = 1; i < argc; ++i) {
//		v.push_back(atoi(argv[i]));	
//	}
	std::cout << timestamp(tstr) << " Before Sort " << "\n";
	if(verbose) std::cout << timestamp(tstr) << " vector : {" << v << "}\n";
	if(stdqucksort)
		std::sort(v.begin(), v.end());
	else
		quicksort(v, 0, v.size()-1);
	
	std::cout << timestamp(tstr) << " After Sort" << "\n";
	if(verbose) std::cout << timestamp(tstr) << " vector : {" << v << "}\n";
	vector2file(ofile, v);
	
	if(verbose) std::cout << timestamp(tstr) << " vector : {" << v << "}\n";

	std::cout << timestamp(tstr) << " After  vector2file ""\n";
	return 0;
}
#endif

#if 0

#include <algorithm>
#include <malloc.h>
#include <iostream>
#include <map>
#include <set>
#include <ostream>
#include <utility>
#include <cctype>
#include <cassert>

size_t mstrlen(const char* str) {
	size_t l = 0;
	while(*str++!='\0') l++;]
	return l;
}
void mstrcpy(char* dst, const char* src) {
	char c;
	while((c=(*dst++=*src++))!='\0');
}
int mstrcmp(const char* a, const char* b) {
	while(*a!='\0' && *a==*b) {a++;b++;};
	return *a-*b;
};
void mmemset(void* dst, char c, size_t len) {
	char* cc = (char*)dst;
	while(len-->0) cc[len] = c;
}

class Item {
	public:
	   	Item() : name{nullptr}, size{0} {}
		Item(const char* name) {
			this->name = new char[(size=mstrlen(name))+1];
			//mmemset(this->name, '\0', size+1);
			mstrcpy(this->name, name);
			this->name[size]='\0';
		}

		Item(const Item& o) {
			*this = o;
		}

		Item& operator=(const Item& o) {
			if(this==&o) return *this;
			if(size != o.size) {
				delete[] name;
				name = nullptr;
				size = 0;
				name = new char[o.size+1];
				size = o.size;
			}
			std::copy(o.name, o.name+o.size, name);
			return *this;
		}

		Item(Item&& o) {
			*this = std::move(o);
		}
		Item& operator=(Item&& o) {
			if(this==&o) return *this;
			delete[] name;
			name = std::exchange(o.name, nullptr);
			size = std::exchange(o.size, 0);
			return *this;
		}

		friend std::ostream& operator<<(std::ostream& os, const Item& o) {
			os << "{\"size\":"<<o.size << ", \"name\":" << o.name << "}";
			return os;
		}
		friend std::istream& operator>>(std::istream& is, Item& o) {
			// verify no errors are set, flush tied streams, strip leading
			// whitespace.
			std::istream::sentry sentry(is);
			if (!sentry)
				return is;

			unsigned int size = 0;
			unsigned int tail = 0;
			char* temp = 0;
			int next; // @note int not char (to record EOF).

//			while ((next = is.get()) != is.eof() && !std::isspace(next)) {
			while ((next = is.get()) != is.eof() && '\n' !=next) {
				// if temp buffer is exhausted, then double the buffer size.
				// (base size of 16).
				if (tail == size) {
					unsigned int newsize = std::max(2*size, 16u);
					char* newtemp = new char[newsize+1];
					memcpy(newtemp, temp, size);
					delete [] temp;
					temp = newtemp;
					size = newsize;
				}
				temp[tail++] = next;
			}
			// @note because the stream is prepeared with istream::sentry, there
			// will be at least one non-whitespace character in the stream.
			assert(temp != NULL);
			temp[tail] = '\0';
			o.append(temp); // append 2021-12-04
			//o = temp;
			delete [] temp;
			return is;
		}
		void append(const char* str)  {
			size_t str_len = mstrlen(str);
			//char* newname = new char[size+str_len+1]{'\0'};
			size_t newsize = size+str_len;
			char* newname = new char[newsize+1];
			assert(newname != NULL);
			if(name != NULL) mstrcpy(newname, name);
			mstrcpy(newname+size, str);
			newname[newsize] = '\0';
			if(name) delete[] name;

			name = nullptr;
			size = 0;

			name = newname;
			size = newsize;

		}
		~Item() {
			if(name)delete[] name;
			name = nullptr;
			size = 0;
		}
	private:
		char* name;
		size_t size;
};

class AA {
	friend std::less<AA>;
	friend std::ostream;
	public:
		AA() : key(nullptr), val(nullptr) {}
		AA(const char* key, const char* val) {
			docopy(key, val);
		}

		// Copy Constructor
		AA(const AA& o) {
			docopy(o.key, o.val);
		}

		// Copy Allocator
		const AA& operator=(const AA& o) {
			if(this==&o) return *this;
			doclear();
			docopy(o.key, o.val);
			return *this;
		}

		// Move Constructor
		AA(AA&& o) {
			key = o.key;
			val = o.val;
			o.key = o.val = nullptr;
		}

		AA& operator=(AA&& o) {
			if(this==&o) return *this;
			doclear();
			key = o.key;
			val = o.val;
			o.key = o.val = nullptr;
			return *this;
		}

		~AA() {
			doclear();
		}

		void doclear() {
			if(key) free(key);
			if(val) free(val);
		}

		void docopy(const char* key, const char* val){
			this->key = (char*)malloc(mstrlen(key)+1);	
			this->val = (char*)malloc(mstrlen(val)+1);

			mstrcpy(this->key, key);
			mstrcpy(this->val, val);
		}
		bool operator <(const AA& o) const {
			return mstrcmp(this->key, o.key) < 0;
		}
		friend std::ostream& operator<<(std::ostream& os, const AA& o);

		const char* getKey() const { return key; }
		const char* getVal() const { return val; }

	private:
		char* key;
		char* val;
};

std::ostream& operator<<(std::ostream& os, const AA& o) {
	os<<"{"<<o.key<<","<<o.val<<"}";
	return os;
}

namespace std
{
	template<> struct less<const char*>
	{
		bool operator() (const char* a, const char* b) const {
			return mstrcmp(a, b) < 0;
		}
	};
	template<> struct less<AA>
	{
		bool operator() (const AA& a, const AA& b) const {
			return mstrcmp(a.key, b.key) < 0;	
		}
	};
}

using namespace std;

int main(int argc, char* argv[]) {

	std::map<const char*, AA> mm;
	std::set<AA> ss;
	for (int i = 1; i<argc; i++) {
		if(i%2==1 && i+1<argc) {
			AA a{argv[i], argv[i+1]};
			cout << "["<<i<<"] : "<<a<<std::endl;
			mm.insert({argv[i], a});
		}
		ss.insert({argv[i], argv[i]});
	}

	cout<<"## 1 const auto& [key,val] : map"<<std::endl;
	for (const auto& [k,v] : mm) {
		cout << "map["<<k<<"]="<< v << std::endl;
	}

	cout<<"## 1-1 const auto& item : set("<<std::endl;
	for (const auto& item : ss) {
		cout << item << ", ";
	}
	cout << ")" << std::endl;

	cout<<"## 2 map[key]=[val]"<<std::endl;
	for(int i = 1;i < argc;i+=2) {
		cout << "map[" << argv[i] << "]" << ":" << mm[argv[i]] << std::endl;
	}
	cout << std::endl;

	cout << "## 3 Item" << std::endl;
	Item ii;
	for(int i = 1;i<argc;i++) {
		if(i>1) ii.append(" ");
		ii.append(argv[i]);
	}
	ii.append(" ");
	cin >> ii;
	cout << "## 3 ii:" <<  ii << std::endl;

	return 0;
}

#endif
