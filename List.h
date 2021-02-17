#pragma once

#include <stdio.h>
#include <malloc.h>

typedef void* Element;
typedef struct _Node Node;
typedef Node* Link;
struct _Node {
	Link prev;
	Link next;
	Element data;
	static Node* newNode(Element data);
	void insert(Link pos, Link node);
	void disconnect();
};

typedef struct _List List;
struct _List {
	Link head;
	Link tail;
	int usage;
	static List* newList();
	void dispose();
	void push_back(Element data);
	void insert(Element data);
	Link begin();
	Link end();
	void erase(Link pos);
	bool find(Element data, int (*comp)(Element val));
};



