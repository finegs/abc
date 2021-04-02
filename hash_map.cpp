#include "hash_map.hpp"

template<typename TKey, typename TValue>
MyHashMap<TKey, TValue>::MyHashMap() {
}
template<typename TKey, typename TValue>
int MyHashMap<TKey, TValue>::CalcHashFunc(TKey key) {
    return key % MAX_COUNT;
}
template<typename TKey, typename TValue>
void MyHashMap<TKey, TValue>::Insert(TKey key, TValue value) {
    std::list<Data<TKey, TValue>*> &list = bucket[CalcHashFunc(key)];
    if (list.size() > 0) {
        for (Data<TKey, TValue> *it : list) {
            if (it->key == key) {
                it->value = value;
                return;
            }
        }
    }
    Data<TKey, TValue>* data = new Data<TKey, TValue>();
    data->key = key;
    data->value = value;
    list.push_back(data);
}
template<typename TKey, typename TValue>
void MyHashMap<TKey, TValue>::Print() {
    for (int i = 0; i < MAX_COUNT; i++) {
        std::list<Data<TKey, TValue>*> list = bucket[i];
        if (list.size() > 0) {
            for (Data<TKey, TValue> *it : list) {
                std::cout << "key : " << it->key << " value : " << it->value << "\t";
            }
        }
        std::cout << std::endl;
    }
}
template<typename TKey, typename TValue>
void MyHashMap<TKey, TValue>::Delete(TKey key) {
    std::list<Data<TKey, TValue>*> &list = bucket[CalcHashFunc(key)];
    if (list.size() > 0) {
        for (Data<TKey, TValue> *it : list) {
            if (it->key == key) {
                list.remove(it);
                return;
            }
        }
    }
}
 
