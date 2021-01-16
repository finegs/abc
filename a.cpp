#if 1

typedef struct _Node Node;
typedef struct _LinkedList LinkedList;

typedef struct _Node {
	void* data;
	Node* next;
	Node* prev;
} Node;

typedef struct _LinkedList {
	size_t size;
	size_t capa;
	Node* head;
	Node* last;
	Node* cur;

	void addFirst(void* data) {
		Node* node = (Node*)malloc(sizeof(Node));
		if(!node) {
			perror("fail to malloc to new Node");
		}
		node->data = data;

		if(head) {
			head->prev = node;
			node->next = head;
			node->prev = NULL;
		}
		else {
			head = node;
			head->prev = NULL;
		}
		size++;
	}
	void add(Node* node, Node* newNode)
	{
		if(!node) return;
		node->prev->next = newNode;
		node->prev = newNode;
		newNode->next = node;
		size++;
	}
	void removeFirst(void* data);
	void remove(Node* node);
	Node* find(Node* data);
	Node* begin();
	Node* end();
	int hasNext();
} LinkedList;
#endif



#if 0

#include <iostream>
#include <cstdint>
#include <complex>

class AA {
	public:
		int a;
};

using namespace std;

int main() {
	uint8_t a = 5, b = 9, c= 255;
	printf("a=%u, b=%u, c=%u\n", a,b,c);

	double complex z1 = 1.0 + 3.0*I;
	printf("z1 = %.02f + %.02fi\n", crealf(z1), cimagf(z1));
//	double imaginary z = 3*I;
//	z = 1/z;
//	printf("1/(3.0i) = %+.lfi\n", cimag(z));
	return 0;
}

#endif
