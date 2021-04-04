#include "hash_map.hpp"

template<typename TKey, typename TValue, size_t Bucket_len>
MyHashMap<TKey, TValue, Bucket_len>::MyHashMap() {
}

template<typename TKey, typename TValue, size_t Bucket_len>
int MyHashMap<TKey, TValue, Bucket_len>::CalcHashFunc(TKey key) {
    return key % Bucket_len;
}
template<typename TKey, typename TValue, size_t Bucket_len>
void MyHashMap<TKey, TValue, Bucket_len>::Insert(TKey key, TValue value) {
    std::list<MyHashMap<TKey, TValue, Bucket_len>::Data*> &list = bucket[CalcHashFunc(key)];
    if (list.size() > 0) {
        for (MyHashMap<TKey, TValue, Bucket_len>::Data *it : list) {
            if (it->key == key) {
                it->value = value;
                return;
            }
        }
    }
    MyHashMap<TKey, TValue, Bucket_len>::Data* data = new MyHashMap<TKey, TValue, Bucket_len>::Data();
    data->key = key;
    data->value = value;
    list.push_back(data);
}
template<typename TKey, typename TValue, size_t Bucket_len>
void MyHashMap<TKey, TValue, Bucket_len>::Print() {
    for (int i = 0; i < Bucket_len; i++) {
        std::list<MyHashMap<TKey, TValue, Bucket_len>::Data*> list = bucket[i];
        if (list.size() > 0) {
            for (MyHashMap<TKey, TValue, Bucket_len>::Data *it : list) {
                std::cout << "key : " << it->key << " value : " << it->value << "\t";
            }
        }
        std::cout << std::endl;
    }
}
template<typename TKey, typename TValue, size_t Bucket_len>
void MyHashMap<TKey, TValue, Bucket_len>::Delete(TKey key) {
    std::list<MyHashMap<TKey, TValue, Bucket_len>::Data*> &list = bucket[CalcHashFunc(key)];
    if (list.size() > 0) {
        for (MyHashMap<TKey, TValue, Bucket_len>::Data* it : list) {
            if (it->key == key) {
                list.remove(it);
                return;
            }
        }
    }
}
 
