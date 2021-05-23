#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>

int main(int argc, char* argv[]) {
	int a,b;
	if(argc<2) return 0;
	a = atoi(argv[1]);
	b = atoi(argv[2]);
//	scanf("%d", a);
//	scanf("%d", &b);
	printf("%d^%d=%d\n", a,b,a^b);
	system("pause");
}

#if 0
typedef struct {
	int no;
	char name[32];
} Book;

int swapn(void* a, void* b, size_t len);
int swapbn(void* a, void* b, void* buf, size_t len);

int main(int argc,char* argv[]) {

	size_t arr_len = 10;
	Book** arr = (Book**)malloc(sizeof(Book*)*arr_len);

	for(size_t i = 0;i<arr_len;i++) {
		*(arr+i) = (Book*)malloc(sizeof(Book));
		memset(*(arr+i), 0, sizeof(Book));
	}
	//Book* arr = (Book*)malloc(sizeof(Book)*arr_len);
	//memset(arr, '\0', sizeof(Book)*arr_len);

	arr[0]->no = 1;
	strcpy(arr[0]->name, "name 1");

	arr[1]->no = 2;
	strcpy(arr[1]->name, "name 2");
	Book bb;

	swapbn(arr[0], arr[1], &bb, sizeof(Book));

	for (size_t i = 0; i < arr_len; i++) {
		printf("[%d] : no=%d, name=%s\n", i, arr[i]->no, arr[i]->name);
	}

	for (size_t i = 0; i < arr_len; i++) {
		free(*(arr+i));
	}
	free(arr);

	return 0;
}

int swapn(void* a, void* b, size_t len) {
	char* aa = (char*)a;	
	char* bb = (char*)b;	
	char* tt = (char*)malloc(len);
	return swapbn(a,b,tt,len);
}

int swapbn(void* a, void* b, void* buf, size_t len) {
	char* aa = (char*)a;	
	char* bb = (char*)b;	
	if(!buf) return 0;
	char* tt = (char*)buf;
	for(int i = 0;i<len;i++) *(tt+i)=*(aa+i);
	for(int i = 0;i<len;i++) *(aa+i)=*(bb+i);
	for(int i = 0;i<len;i++) *(bb+i)=*(tt+i);
	return 0;
}

#endif
