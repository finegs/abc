#include <stdio.h>
#include <malloc.h>
#include "List.h"


int main() {
	List list;	

	list.insert(10);
	list.insert(20);
	list.insert(30);
	list.insert(40);
	list.insert(50);
	list.insert(56);

	int a;
	list.remove(&a);

	list.printList();

	return 0;
}
