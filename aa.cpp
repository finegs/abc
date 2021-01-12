#include <stdio.h>
#include <stdlib.h>

struct MyData {
	int data;
};

struct Node {
	int data;
	Node *prev;
	Node *next;
};

struct List {
	Node *head = NULL;
	Node *tail = NULL;
	Node *current = NULL;
	
	void printList() {
		Node* ptr = head;

		printf("\n[head]=>");
		while(ptr) {
			printf("%s %d : ", ((head != ptr) ? ", " : ""),  ptr->data);
			ptr = ptr->next;
		}
		printf(" [null]\n");
	}

	Node* begin() { return head; }
	Node* end() { return tail; }

	void insert(int data) {
		Node* newNode = (Node*)malloc(sizeof(Node));
		newNode->data = data;
		
		if(head) {
			newNode->next = head;
			newNode->next->prev = newNode;
		}

		head = newNode;
		if(!tail) tail = newNode;
	}	

	void insertEnd(int data) {
		Node* newNode = (Node*)malloc(sizeof(Node));
		newNode->data = data;

		if(tail) {
			tail->next = newNode;
			newNode->prev = tail;

			tail = newNode;
		}
		else {
			head = newNode;
			tail = newNode;
		}
	}

	void remove(int* data) {
		if(head) {
			*data = head->data;

			Node* t = head;

			head = head->next;

			t->prev = NULL;
			free(t);
		}
	}

	void removeEnd(int* data) {
		if(tail) {
			*data = tail->data;

			Node* t = tail;
			tail = t->prev;
			t->prev->next = NULL;

			free(t);
			t = NULL;
		}	
	}

};


int main() {
	List l;	

	l.insert(10);
	l.insert(20);
	l.insert(30);
	l.insert(40);
	l.insert(50);
	l.insert(56);

	int a;
	l.remove(&a);

	l.printList();

	return 0;
}
