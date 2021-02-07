#include <stdio.h>
#include <malloc.h>

#include "List.h"


void List::printList() {
 	Node* ptr = head;
 
 	printf("\n[head]=>");
 	while(ptr) {
 		printf("%s %d : ", ((head != ptr) ? ", " : ""),  ptr->data);
 		ptr = ptr->next;
 	}
 	printf(" [null]\n");
}

Node* List::begin() { return head; }
Node* List::end() { return tail; }

void List::insert(int data) {
	Node* newNode = (Node*)malloc(sizeof(Node));
	newNode->data = data;
	
	if(head) {
		newNode->next = head;
		newNode->next->prev = newNode;
	}

	head = newNode;
	if(!tail) tail = newNode;
}	

void List::insertEnd(int data) {
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

void List::remove(int* data) {
	if(head) {
		*data = head->data;

		Node* t = head;

		head = head->next;

		t->prev = 0;
		free(t);
	}
}

void List::removeEnd(int* data) {
	if(tail) {
		*data = tail->data;

		Node* t = tail;
		tail = t->prev;
		t->prev->next = 0;

		free(t);
		t = 0;
	}	
}

