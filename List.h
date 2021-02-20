#pragma once

struct MyData {
	int data;
};


struct Node {
	int data;
	Node *prev;
	Node *next;
};

struct List {
	Node *head = 0;
	Node *tail = 0;
	Node *current = 0;
	
	void printList(); 

	Node* begin();
	Node* end();

	void insert(int data);	
	void insertEnd(int data); 
	void remove(int* data); 
	void removeEnd(int* data); 
};

