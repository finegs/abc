#pragma once
#include <iostream>
#include <list>
 
template<typename TKey, typename TValue, size_t Bucket_len = 1000>
class MyHashMap {
private:

//	template<typename TTKey, typename TTValue>
	struct Data;
//	{
//		TKey key;
//		TValue value;
//	};
//    static int const MAX_COUNT = 5;
//  std::list<MyHashMap<TKey, TValue>::Data*> bucket[MAX_COUNT];
    std::list<typename MyHashMap<TKey, TValue>::Data*> bucket[Bucket_len];
    int CalcHashFunc(TKey key);
public:
    MyHashMap<TKey, TValue, Bucket_len>();
    void Insert(TKey key, TValue value);
    void Delete(TKey key);
    void Print();
};
 
template<typename TKey, typename TValue, size_t Bucket_len>
struct MyHashMap<TKey, TValue, Bucket_len>::Data {
	TKey key;
	TValue value;
};
