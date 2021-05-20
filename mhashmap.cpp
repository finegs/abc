#include<vector>
#include<list>
#include<string>
#include"iostream"
 
using namespace std;
 
class IntHash
{
public:
	int operator()(const int &value) { return value; }
};
class IntCmp
{
public:
	int operator()(const int &a, const int &b) { return a==b; }
};
 
template<class _Key,class _Value, class _HashFun, class _Cmp>
class hashMap
{
typedef pair<_Key, _Value> HashPair; //hash key-value pair
typedef list<HashPair> HashList; //Hash list (handling conflicts)
typedef vector<HashList> HashTable; //hash table
 
public:
	hashMap(int size):m_size(size)
	{
	hash_table.resize(size);
	}
	void add(const _Key &key, const _Value &value)
	{
		int index = hashFun(key) % m_size;
 
		HashList &list = hash_table[index];
 
		list.push_back(pair<_Key, _Value>(key, value));
	}
	_Value find(const _Key &key)
	{
		int index = hashFun(key) % m_size;
 
		HashList &list = hash_table[index];
 
		auto it = list.begin();
		
		while (it != list.end())
		{
			if (hashCmp(it->first, key))
			{
				return it->second;
			}
			
			it++;
		}

		return NULL;
	}
 
private:
	int m_size;
	HashTable hash_table;
	
	_HashFun hashFun;
	_Cmp hashCmp;
};
 
int main()
{
	hashMap<int, string, IntHash, IntCmp> map(100);
	
	map.add(9527, "9527");
	map.add(135, "135");
	map.add(2, "2");
	map.add(567, "567");
	map.add(888, "888");
	map.add(3669, "3669");
	
	cout << map.find(9527) << endl;
	cout << map.find(567) << endl;
}
