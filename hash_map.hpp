#pragma once

#include <iostream>

template<typename K, size_t TableSize=1000000>
struct KeyHash {
	unsigned long operator() (const K& key) const  {
		return reinterpret_cast<unsigned long>(key) % TableSize;
	}
};	

template <typename Key, typename Value>
class HashNode {
	public:
		HashNode(Key key, Value value) : key(key), value(value) , next(nullptr) {}
		Key getKey() const { return key; }
		Value getValue() const{ return value; }
		HashNode* getNext() const { return next; }
		void setValue(Value value) { HashNode::value = value; }
		void setNext(HashNode* next) { HashNode::next = next; }

   	private:
		Key key;
		Value value;
		HashNode* next;
};

template <typename Key, typename Value, typename Hash, size_t TableSize = 1000000>
class HashMap {
	public:
		HashMap() {
			table = new HashNode<Key,Value>* [TableSize];
		}

		~HashMap() {
			for (int i = 0; i < TableSize; ++i) {
				HashNode<Key,Value> *entry = table[i];
				while(entry != NULL) {
					HashNode<Key,Value> *pre = entry;
					entry = entry->getNext();
					delete pre;
				}
				table[i] = nullptr;
			}
			delete[] table;
		}

		bool get(const Key &key, Value &value) {
			unsigned long hashValue = hashFunc(key);
			HashNode<Key, Value>* entry = table[hashValue];
			
			while(entry != nullptr) {
				if(entry->getKey() == key) {
					value = entry->getValue();
					return true;
				}
				entry = entry->getNext();
			}
			return false;
		}

		void put(const Key& key, const Value& value) {
			unsigned long hashValue = hashFunc(key);
			HashNode<Key, Value> *prev = nullptr;
			HashNode<Key, Value> *entry = table[hashValue];

			while(entry != nullptr && entry->getKey() != key) {
				prev = entry;
				entry = entry->getNext();
			}
			if(entry==nullptr){
				entry = new HashNode<Key, Value>(key, value);
				if(prev==nullptr) {
					table[hashValue] = entry;
				} else {
					prev->setNext(entry);
				}
			}
			else {
				entry->setValue(value);
			}
		}

		void remove(const Key& key) {
			unsigned long hashValue = hashFunc(key);
			HashNode<Key, Value> *prev = nullptr;
			HashNode<Key, Value> *entry = table[hashValue];

			while(entry != nullptr&& entry->getKey() != key) {
				prev = entry;
				entry = entry->getNext();
			}

			if(entry==nullptr) {
				return;
			}
			else {
				if(prev==nullptr) {
					table[hashValue] = entry->getNext();
				}
				else {
					prev->setNext(entry->getNext());
				}
				delete entry;
			}
		}

	private:
		HashNode<Key, Value>** table;
		Hash hashFunc;

};


