#pragma once

#include<stdio.h>
#include<malloc.h>

typedef void* Element;
typedef struct Node_ Node;
typedef Node* Link;
struct Node_{
	Link prev;
	Link next;
	Element data;
	static Link newNode(Element data);
	void dispose();
	void connect(Link pos);
	void disconnect();
};

typedef struct List_ List;
struct List_ {
	Link head;
	Link tail;
	int usage;
	static List* newList();
	void dispose();
	Link begin();
	Link end();
	void push_back(Element data);
	void insert(Link pos, Element data);
	void remove(Link node);
	void view_all(void (*view)(Element data));
};


