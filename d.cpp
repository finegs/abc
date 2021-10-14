#include <algorithm>
#include <malloc.h>
#include <iostream>
#include <map>
#include <ostream>

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
}

using namespace std;

int main(int argc, char* argv[]) {

	std::map<const char*, AA> mm;
	for (int i = 1; i<argc && i+1<argc; i+=2) {
		AA a{argv[i], argv[i+1]};
		cout << "["<<i<<"] : "<<a<<std::endl;
		mm.insert({argv[i], a});
	}

	cout<<"##1 const auto& [key,val]"<<std::endl;
	for (const auto& [k,v] : mm) {
		cout << "map["<<k<<"]="<< v << std::endl;
	}

	cout<<"##2 map[key]=[val]"<<std::endl;
	for(int i = 1;i < argc;i+=2) {
		cout << "map[" << argv[i] << "]" << ":" << mm[argv[i]] << std::endl;
	}
	cout << std::endl;

	return 0;
}
