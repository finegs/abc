#pragma once

#include <iostream>

#define HASH_MAP_USE_KEYSET 0

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
#if HASH_MAP_USE_KEYSET
			keySet = nullptr;
#endif
		}

		~HashMap() {
#if 1
			for (int i = 0; i < TableSize; ++i) {
			 	HashNode<Key,Value> *entry = table[i];
#endif
#if HASH_MAP_USE_KEYSET
			for (HashNode<Key,int> *keyItem = keySet; keyItem;keyItem=keyItem->getNext()) {
				HashNode<Key,int> *keyItemPrev = keyItem;
				HashNode<Key,Value> *entry = table[keyItem->getValue()];

#endif
				while(entry != NULL) {
					HashNode<Key,Value> *pre = entry;
					entry = entry->getNext();
					delete pre;
				}
#if 1	
				table[i] = nullptr;
#endif
#if HASH_MAP_USE_KEYSET
				table[keyItem->getValue()] = nullptr;
				delete keyItemPrev;
#endif
			}
			delete[] table;
			table = nullptr;
#if HASH_MAP_USE_KEYSET
			keySet = nullptr;
#endif
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
			
			// add newKey to keySet List
#if HASH_MAP_USE_KEYSET
			HashNode<Key,int>* newKey = new HashNode<Key, int>(key, hashValue);
			if(keySet) {
				newKey->setNext(keySet);
				keySet = newKey;
			}
			else {
				keySet = newKey;
				keySet->setNext(nullptr);
			}
#endif
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

		void print() {

			HashNode<Key, Value> *prev;
			HashNode<Key, Value> *entry;

			for(int i = 0;i<TableSize;i++) {

				prev = nullptr;
				entry = table[i];

				if(!entry) continue;

				printf("[%d] : ", i);

				while(entry != nullptr) {
					printf("(%s, %s)", entry->getKey(), entry->getValue());	
					entry = entry->getNext();	
					if(entry) printf(",");
				}
				printf("\n");
			}
		}

		void clearall() {
			for (int i = 0; i < TableSize; ++i) {
			 	HashNode<Key,Value> *entry = table[i];

				while(entry != NULL) {
					HashNode<Key,Value> *pre = entry;
					entry = entry->getNext();
					delete pre;
				}
				table[i] = nullptr;
			}
		}

	private:
		HashNode<Key, Value>** table;
#if HASH_MAP_USE_KEYSET
		HashNode<Key, int>* keySet;
#endif
		Hash hashFunc;
};


