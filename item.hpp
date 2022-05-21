#ifndef ITEM_HPP_C84IZPJQ
#define ITEM_HPP_C84IZPJQ

#include <iostream>
#include <vector>
#include <unordered_map>

struct Item {
	int x,y;

	Item() : x{0}, y{0} {}
	Item(int _x, int _y) : x{_x}, y{_y} {}
	~Item() = default;
	Item(const Item& o) : Item() { 
		*this = o; 
	}	
	Item& operator=(const Item& o) { 
		if(this==&o) return *this; 
		x = o.x; 
		y = o.y; 
		return *this;
	}

	friend std::ostream& operator<<(std::ostream& os, const Item& o);

	bool operator==(const Item& o) const {
		return x == o.x && y == o.y;
	}
};

template<> struct std::less<Item>
{
	bool operator()(const Item& a, const Item& b) const 
	{
		return a.x < b.x || a.y < b.y;
	}
};
template<> struct std::hash<Item> 
{
	size_t operator()(const Item& o) const
	{
		return (o.x<<5)&o.y;
	}
};

std::ostream& operator<<(std::ostream& os, const Item& o) {
	os << "{" << "\"x\":"<< '"' << o.x << '"' << ", " << "\"y\":" << '"' << o.y <<'"'<< "}";
	return os;
}

//template <typename Container,
//		 std::enable_if_t<std::is_same<decltype(std::declval<Container>().begin()),
//		 decltype(std::declval<Container>().begin())>::value>* = nullptr>
//std::ostream& operator<<(std::ostream& os, const Container& v)
template<typename T>
std::ostream& operator<<(std::ostream& os, std::vector<T>& v)
{
	char sep[3]{'\0', ' ', '\0'};
	os << "[";
	for(auto& o:v) { 
		os << sep << o; 
		sep[0] = ','; 
	}
	os << "]";
	return os;
}

template<typename K, typename V>
std::ostream& operator<<(std::ostream& os, std::unordered_map<K,V>& m) {
	char sep[3]{'\0', ' ', '\0'};
	os << "[";
	os << "size:" << m.size();
	os << ",";
	for(auto& o: m) {
		os << sep << '{'<< '"' << o.first << '"' << ':' << o.second << '}';
		sep[0] = ',';
	}
	os << "]";
	return os;
}


#endif /* end of include guard: ITEM_HPP_C84IZPJQ */
