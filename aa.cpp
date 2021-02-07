#include <stdio.h>
#include <malloc.h>
#include "List.h"
#include "Util.h"


int main() {
	List list;	

	list.insert(10);
	list.insert(20);
	list.insert(30);
	list.insert(40);
	list.insert(50);
	list.insert(56);

	int a;
//	list.remove(&a);

	list.printList();

	size_t str_len = 1024+1;
	char str[str_len];
	mmemset(str, 0, str_len);

	printf("string : "); fflush(stdout);
	scanf("%s", str); fflush(stdin);
	printf("hash(%s) : %d\n", str, strhash(str));

	int n;
	printf("number : "); fflush(stdout);
	scanf("%d", &n); fflush(stdin);
	printf("prime_max(%d) : %d\n", n, prime_max(n));


	return 0;
}
