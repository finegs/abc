#if 1

#include <iostream>
#include <new>
#include <string>

#include <cstdio>
#include <cstdlib>
#include <cstring>

class MyClass {
    public:
    int data[100];
    MyClass() { std::cout << "constructed {" << this << "}\n";}
};

int main() {
    std::cout << "1 : ";
    MyClass* p1 = new MyClass;
    std::cout << "\n";
    
    std::cout << "2 : ";
    MyClass* p2 = new (std::nothrow) MyClass;
    std::cout << "\n";

    std::cout << "3 : ";
    MyClass* p2_1 = new (p2) MyClass;
    std::cout << "\n";

    std::cout << "4: ";
    MyClass* p3 = (MyClass*) ::operator new(sizeof(MyClass));
    std::cout << "\n";

    delete p1;
    if(p2_1) {
        delete p2_1;
        p2_1 = nullptr;
        p2 = nullptr;
    }
    if(p2) {
        delete p2;
    }
    delete p3;
#if 0
    int n = 100;
    char* cp;

    std::string* strp = nullptr;

    cp = new char[10];
    memset(cp, '\0', 10);
    memcpy(cp, "Good CP", 7);
    strp = new std::string("Good Job");

    char array[sizeof(std::string)];
    std::string* strpp= new(array) std::string{"Good Luck"};

    std::cout << "*cp : " << *cp << std::endl;
    std::cout << "*strp : " << *strp << std::endl;
    std::cout << "*strpp : " << *strpp << std::endl;
    std::cout << "sizeof(std::string) : " << sizeof(std::string) << std::endl;

    delete[] cp;
    delete strp;
    delete[] strpp;

    std::cout << "### End of Main ###" << std::endl;
#endif

    return 0;
}


#endif
#if 0

#include <iostream>
#include <chrono>
#include <map>
#include <unordered_map>
#include <string>
#include <vector>
#include <cstdlib>
#include <random>

char randchar() {
    static std::mt19937_64 rng(0xbeef);
    std::uniform_int_distribution<> dist(1,255);
    return static_cast<char>(dist(rng));
}

std::vector<std::string> prepare(int len, int count) {
    std::vector<std::string> r;
    r.reserve(count);
    for (int i = 0; i < count; i++)
    {
        std::string s;
        s.reserve(len);
        for (int si= 0; si < count; ++si)
        {
            s += randchar();
        }
        r.push_back(std::move(s));
    }
    return r;
}

template<typename map_t> void run(int len, int count) {
    using mapped_type = typename map_t::mapped_type;
    std::vector<std::string> srces = prepare(len, count);
    auto start = std::chrono::high_resolution_clock::now();
    map_t map;
    for(auto &&s:srces) {
        map.emplace(std::move(s), mapped_type());
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << std::chrono::duration_cast<std::chrono::microseconds>(end-start).count() * len*3
    << "ms" << std::endl;
}

int main(int argc, const char* argv[]) {
    if(argc < 3) {
        std::cout << "Usage : " << argv[0] << " {type:u,m} {len} {count}" << std::endl;
        return 0;
    }
    char type = *argv[1];
    int len = std::atoi(argv[2]);
    int count = std::atoi(argv[3]);
    switch(type) {
        case 'm':
        run<std::map<std::string, int>>(len, count);
        break;
        
        case 'u':
        run<std::unordered_map<std::string, int>>(len, count);
        break;

        default:
        std::cout << "unsuported type : " << type << std::endl;

    }

    return 0;
}

#endif 

#if 0
#include <iostream>
#include <string>

// Hash node class template
template <typename K, typename V>
class HashNode {
public:
    HashNode(const K &key, const V &value) :
    key(key), value(value), next(NULL) {
    }

    K getKey() const {
        return key;
    }

    V getValue() const {
        return value;
    }

    void setValue(V value) {
        HashNode::value = value;
    }

    HashNode *getNext() const {
        return next;
    }

    void setNext(HashNode *next) {
        HashNode::next = next;
    }

private:
    // key-value pair
    K key;
    V value;
    // next bucket with the same key
    HashNode *next;
};

// Default hash function class
template<typename K, int TABLE_SIZE = 1048576>
struct KeyHash {
	unsigned long operator()(const K& key) const {
		return reinterpret_cast<unsigned long>(key) % TABLE_SIZE;
	}
};

// Hash map class template
template <typename K, typename V, typename F = KeyHash<K>, int TABLE_SIZE = 1048576>
class HashMap {
public:
    HashMap() {
        // construct zero initialized hash table of size
        table = new HashNode<K, V> *[TABLE_SIZE]();
    }

    ~HashMap() {
        // destroy all buckets one by one
        for (int i = 0; i < TABLE_SIZE; ++i) {
            HashNode<K, V> *entry = table[i];
            while (entry != NULL) {
                HashNode<K, V> *prev = entry;
                entry = entry->getNext();
                delete prev;
            }
            table[i] = NULL;
        }
        // destroy the hash table
        delete [] table;
    }

    bool get(const K &key, V &value) {
        unsigned long hashValue = hashFunc(key);
        HashNode<K, V> *entry = table[hashValue];

        while (entry != NULL) {
            if (entry->getKey() == key) {
                value = entry->getValue();
                return true;
            }
            entry = entry->getNext();
        }
        return false;
    }

    void put(const K &key, const V &value) {
        unsigned long hashValue = hashFunc(key);
        HashNode<K, V> *prev = NULL;
        HashNode<K, V> *entry = table[hashValue];

        while (entry != NULL && entry->getKey() != key) {
            prev = entry;
            entry = entry->getNext();
        }

        if (entry == NULL) {
            entry = new HashNode<K, V>(key, value);
            if (prev == NULL) {
                // insert as first bucket
                table[hashValue] = entry;
            } else {
                prev->setNext(entry);
            }
        } else {
            // just update the value
            entry->setValue(value);
        }
    }

    void remove(const K &key) {
        unsigned long hashValue = hashFunc(key);
        HashNode<K, V> *prev = NULL;
        HashNode<K, V> *entry = table[hashValue];

        while (entry != NULL && entry->getKey() != key) {
            prev = entry;
            entry = entry->getNext();
        }

        if (entry == NULL) {
            // key not found
            return;
        }
        else {
            if (prev == NULL) {
                // remove first bucket of the list
                table[hashValue] = entry->getNext();
            } else {
                prev->setNext(entry->getNext());
            }
            delete entry;
        }
    }

private:
    // hash table
    HashNode<K, V> **table;
    F hashFunc;
};

struct MyKeyHash {
    unsigned long operator()(const int& k) const
    {
        return k % 10;
    }
};

using namespace std;

int main() {

	HashMap<int, string, MyKeyHash> hmap;
	hmap.put(1, "val1");
	hmap.put(2, "val2");
	hmap.put(3, "val3");
	
	string value;
	hmap.get(2, value);
	cout << value << endl;
	bool res = hmap.get(3, value);
	if (res)
		cout << value << endl;
	hmap.remove(3);
	res = hmap.get(3, value);
	if (res)
		cout << value << endl;

	return 0;
}

#endif