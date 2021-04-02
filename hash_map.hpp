#pragma once
#include <iostream>
#include <list>
 
template<typename TKey, typename TValue>
class MyHashMap {
private:
    template<typename TTKey, typename TTValue>
    struct Data
    {
        TKey key;
        TValue value;
    };
    static int const MAX_COUNT = 5;
    std::list<Data<TKey, TValue>*> bucket[MAX_COUNT];
    int CalcHashFunc(TKey key);
public:
    MyHashMap();
    void Insert(TKey key, TValue value);
    void Delete(TKey key);
    void Print();
};
 

