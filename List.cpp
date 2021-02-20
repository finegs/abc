#include "List.h"

Node newNode(Element data) {
	Link link = (Link)malloc(sizeof(Node));
	link->data = data;
	link->prev = link->next = 0;
	return link;
}

void Node::insert(Link node) {
	node->prev =  pos->prev;
	node->next = pos;
	prev->next = node;
	prev  = now;
}

void Node::disconnect() {
	prev->next = next;
	next->prev = prev
}

List* newList() {
	List* list = 0;

	list = (List*)malloc(sizeof(List));
	list->head = newNode(0);
	list->tail = newNode(0);
	list->head->next = tail;
	list->tail->prev = head;
	list->usage = 0;

	return list;
}

void List::dispose() {
	Link cur = head;
	while(cur != tail) {
		cur = cur->next;
		free(cur->prev);
	}
	free(tail);
	free(this);
}

Link List::head() { return head; }
Link List::tail() { return tail; }

void List::insert(Element data, Link pos = head) {
	Link node = (Link)malloc(sizeof(Link));

	node->next = pos->next;
	node->prev = pos->prev;
	pos->prev->next = node;
	pos->next->prev = node;
}
void Elemet::dispose() {
	free(data);
}


