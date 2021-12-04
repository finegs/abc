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
	while(*str++!='\0') l++;
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
