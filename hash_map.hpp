#pragma once
#include <iostream>
#include <list>
template<typename Key, typename Value>
struct MyHashMapEntry {
	Key key;
	Value value;
};

template<typename TKey, typename TValue, size_t Bucket_len = 1000>
class MyHashMap {
private:

    std::list<MyHashMapEntry<TKey, TValue>*> bucket[Bucket_len];
    
    static int const MAX_COUNT = 5;
    int CalcHashFunc(TKey key);

public:
    MyHashMap<TKey, TValue, Bucket_len>();
    void Insert(TKey key, TValue value);
    void Delete(TKey key);
    void Print();
};

template<typename TKey, typename TValue, size_t Bucket_len>
MyHashMap<TKey, TValue, Bucket_len>::MyHashMap() {
}

template<typename TKey, typename TValue, size_t Bucket_len>
int MyHashMap<TKey, TValue, Bucket_len>::CalcHashFunc(TKey key) {
    return key % Bucket_len;
}
template<typename TKey, typename TValue, size_t Bucket_len>
void MyHashMap<TKey, TValue, Bucket_len>::Insert(TKey key, TValue value) {
    std::list<MyHashMapEntry<TKey, TValue>*> &list = bucket[CalcHashFunc(key)];
    if (list.size() > 0) {
        for (MyHashMapEntry<TKey, TValue> *it : list) {
            if (it->key == key) {
                it->value = value;
                return;
            }
        }
    }
    MyHashMapEntry<TKey, TValue>* data = new MyHashMapEntry<TKey, TValue>;
    data->key = key;
    data->value = value;
    list.push_back(data);
}
template<typename TKey, typename TValue, size_t Bucket_len>
void MyHashMap<TKey, TValue, Bucket_len>::Print() {
    for (int i = 0; i < Bucket_len; i++) {
        std::list<MyHashMapEntry<TKey, TValue>*> list = bucket[i];
        if (list.size() > 0) {
            for (MyHashMapEntry<TKey, TValue> *it : list) {
                std::cout << "key : " << it->key << " value : " << it->value << "\t";
            }
        }
        std::cout << std::endl;
    }
}
template<typename TKey, typename TValue, size_t Bucket_len>
void MyHashMap<TKey, TValue, Bucket_len>::Delete(TKey key) {
    std::list<MyHashMapEntry<TKey, TValue>*> &list = bucket[CalcHashFunc(key)];
    if (list.size() > 0) {
        for (MyHashMapEntry<TKey, TValue>* it : list) {
            if (it->key == key) {
                list.remove(it);
                return;
            }
        }
    }
}
 

 
