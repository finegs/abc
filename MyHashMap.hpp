#ifndef __MYHASHMAP_H
#define __MYHASHMAP_H

#include <iostream>

template<typename K, typename V>
class MyHashNode {
	public:
	MyHashNode(const K& key, const V& value)
			: key(key), value(value), next(nullptr) {}

	K getKey() const { return key; }
	V getValue() const { return value; }
	void setValue(V value) { MyHashNode::value = value; }
	MyHashNode* getNext() const { return next; }
	void setNext(MyHashNode* next) { MyHashNode::next = next; }

	private:
	K key;
	V value;
	MyHashNode* next;
};

template <typename K>
struct MyKeyHash {
	unsigned long operator() (const K& key) const {
		return reinterpret_cast<unsigned long>(key);
	}
};

template<typename K, typename V>
struct MyEntryViewer {
	void operator()(FILE* fp, MyHashNode<K,V>& o) {
		fprintf(fp, "{Entry:%p, Key:%p, Value;%p}", &o, &o.getKey(), &o.getValue());
	}
};

template<typename K, typename V, typename Viewer = MyEntryViewer<K,V>, typename F = MyKeyHash<K>, size_t TABLE_SIZE = 1000>
class MyHashMap {
public:
	MyHashMap() {
		table = new MyHashNode<K,V>*[TABLE_SIZE];
		size = 0;
		capacity = TABLE_SIZE;
	}

	~MyHashMap() {
		if(size<1) return;
		for (int i = 0; i < capacity; ++i) {
			MyHashNode<K,V>* entry = table[i];
			if(!entry) continue;
			while(entry) {
				MyHashNode<K,V>* prev = entry;
				entry = entry->getNext();
				delete prev;
			}
			table[i] = nullptr;
		}
		delete[] table;
		size = capacity = 0;
	}


	bool get(const K& key, V& value) {
		if(size<1) return false;
		unsigned long hashValue = hashFunc(key);
		MyHashNode<K,V>* entry = table[hashValue%TABLE_SIZE];
		while(entry) {
			if(entry->getKey() == key) {
				value = entry->getValue();
				return true;
			}
			entry = entry->getNext();
		}
		return false;
	}

	void put(const K& key, const V& value) {
		unsigned long hashValue = hashFunc(key);
		MyHashNode<K,V>* prev = nullptr;
		MyHashNode<K,V>* entry = table[hashValue%TABLE_SIZE];

		while(entry && entry->getKey() != key) {
			prev = entry;
			entry= entry->getNext();
		}

		if(!entry) {
			entry = new MyHashNode<K,V>(key, value);
			if(nullptr == prev) {
				table[hashValue%TABLE_SIZE] = entry;
			}
			else {
				prev->setNext(entry);
			}
		}
		else {
			entry->setValue(value);
		}
		size++;
	}

	void remove(const K& key) {
		unsigned long hashValue = hashFunct(key);
		MyHashNode<K,V>* prev = nullptr;
		MyHashNode<K,V>* entry = table[hashValue%TABLE_SIZE];

		while(entry && entry->getKey() != key) {
			prev = entry;
			entry = entry->getNext();
		}

		if(!entry) {
			// Key not found
			return;
		}
		else {
			if(nullptr == prev) {
				table[hashValue%TABLE_SIZE] = entry->getNext();
			}
			else {
				prev->setNext(entry->getNext());
			}
		}
		size--;
	}

	void print(FILE* fp) {
		fprintf(fp, "{size=%d, capacity=%d", size, capacity);
		fprintf(fp, ", Entry=[");
		for (int i = 0; i < TABLE_SIZE; ++i) {
			MyHashNode<K,V>* entry = table[i];

			if(!entry) continue;

			fprintf(fp,
					"%s{Key=%s, Value=%s}",
						(i>0?", ":""),
						entry->getKey(),
						entry->getValue());
		}
		fprintf(fp, "]}\n");
	}

private:
	MyHashNode<K,V>** table;
	Viewer viewer;
	F hashFunc;
	size_t size;
	size_t capacity;
};

struct MyCharsKeyHash {
	unsigned long operator()(const char* str) const {
		unsigned long h = 5731;
		char* p = (char*)str;
		while(*p) {
			h = (h<<5&h);
			h*=*p;
			p++;
		}
		return h;
	}
};

#endif
