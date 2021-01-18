#include"list.h"

Link newNode(Element data) {
	Link node = (Link)malloc(sizeof(Node));
	node->data = data;
	node->prev = node->next = 0;
	return node;
}

void Node::dispose() {
	free(data);
	// TODO free(prev),free(next)
}

void Node::connect(Link pos) {
	prev = pos->prev;
	next = pos;
	prev->next = this;
	next->prev = this;
}

void Node::disconnect() {
	next->prev = prev;
	prev->next = next;
}

static List* newList() {
	List* list = (List*)malloc(sizeof(List));
	list->head = (Link)malloc(sizeof(Node));
	list->tail = (Link)malloc(sizeof(Node));
	list->head->next = list->tail;
	list->tail->prev = list->head;
	list->usage = 0;
	return list;
}

void List::dispose() {
	Link cur = begin();
	while(cur != end()) {
		cur = cur->next;
		free(cur->prev);	
	}
	free(end());
	free(this);
};


Link List::begin() {
	return head->next;
}

Link List::end() {
	return tail;
}

void List::push_back(Element data){
	insert(end(), data);
}

void List::insert(Link pos, Element data) {
	Link node = newNode(data);
	node->connect(pos);	
	usage++;
}

void List::remove(Link node) {
	node->disconnect();
	node->dispose();
	usage--;
}

void List::view_all(void (*view)(Element data)) {
	for(Link cur = begin();cur != end();cur=cur->next) {
		view(cur->data);		
	}
}

