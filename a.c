#include <stdio.h>
#include <string.h>
#include <malloc.h>

typedef struct {
	int no;
	char name[32];
} Book;

void swap(void* a, void* b);

int main(int argc,char* argv[]) {

	size_t arr_len = 10;
	Book* arr = (Book*)malloc(sizeof(Book)*arr_len);
	memset(arr, '\0', sizeof(Book)*arr_len);

	arr[0].no = 1;
	strcpy(arr[0].name, "name 1");

	arr[1].no = 2;
	strcpy(arr[1].name, "name 2");

	swap(arr, arr+1);

	for (size_t i = 0; i < arr_len; i++) {
		printf("[%d] : no=%d, name=%s\n", i, arr[i].no, arr[i].name);
	}

	free(arr);

	return 0;
}

void swap(void* a, void* b) {
	size_t sz = sizeof(a);
	void** aa = (void**)&a;	
	void** bb = (void**)&b;	
	void* t = *aa;
	*aa = bb;
	*bb = t;
}
