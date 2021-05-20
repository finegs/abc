#include <iostream>

template<typename K, typename V>
class Node{
	public:

	Node(K k, V v) : key(k), value(v) {}

	// for copy
	Node(const Node& o) : key(o.key), value(o.value), next(o.next) {}
	Node operator=(const Node& o) { Node::key = o.key; Node::value = o.value; }
	// for move
	Node(Node&& o) : key(std::move(o.key)), value(std::move(o.value)), next(std::move(o.next)) {}
	Node operator=(Node&& o) { Node::key = std::move(o.key); Node::value = std::move(o.value); Node::next = std::move(o.next); }

	K getKey() const { return Node::key; }
	void setValue(V value) { Node::value = value; }
	V getValue() const { return Node::value; }
	Node* getNext() const { return next; }
	void setNext(Node* next) { Node::value = next; }


	private:
		K key;
		V value;
		Node* next;
};


#if 0
template<typename T>
void f(T s) { std::cout << s << std::endl; }

//template void f<double>(double);
//template void f<unsigned int>(unsigned int);
int main() 
{

	f<int>(100);
	f<size_t>(1000u);

	return 0;
}

#endif
