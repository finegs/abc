#include <stdio.h>
#include <malloc.h>

#define MAX_TABLE_SIZE 10000000

typedef struct _Node {
	char key[512];
	struct _Node* next;
} Node;


typedef struct _Dic {
	struct _Node* head;
}Dic;


Dic dict[MAX_TABLE_SIZE];

int N,M,result = 0;
char word[512];

int hashing(char* word) {
	int hash = 5031, idx = 0;
	while(word[idx] != '\0') {
		hash = (((hash<<5) + hash) * (int)word[idx])%MAX_TABLE_SIZE;
		idx++;
	}
	if(hash<0) return (hash%MAX_TABLE_SIZE)*-1;
	return hash%MAX_TABLE_SIZE;
}


Node* newNode() {
	Node* node = (Node*)malloc(sizeof(Node));
	node->next = NULL;
	return node;
}

size_t strlen(const char* word) {
	size_t len = 0;
	char* cp = (char*)word;
	while(*cp++ != '\0') len++;
	return len;
}


void strcpy(char* dst, const char* src, size_t len) {
	int idx = 0;
	while(idx<len) {
		*(dst+idx) = *(src+idx);
		idx++;
	}
	*(dst+idx)='\0';
}

int strcmp(const char* a, const char* b) {
	char* aa = (char*)a;
	char* bb = (char*)b;
	while(1) {
		if(*aa!=*bb) return *aa > *bb ? 1 : -1;
		if(*aa == '\0') break;
		aa++, bb++;
	}
	return 0;
}

void insertNode(char* word) {
	int hash = hashing(word);
	Node* tmp = newNode();
	strcpy(tmp->key, word, strlen(word));
	if(!dict[hash].head) dict[hash].head = tmp;
	else {
		Node* cur = dict[hash].head;
		while(cur->next) cur = cur->next;
		cur->next=tmp;
	}
}

int findNode(char* word, Node** node) {
	int hash = hashing(word);
	*node = nullptr;
	Node* cur = dict[hash].head;
	if(!cur) return 0;
	else {
		while(cur) {
			if(!strcmp(cur->key, word)) { *node = cur; return 1; }
			else cur = cur->next;
		}
		return 0;
	}
}

int main(int argc, char* argv[]) {
	
	scanf("%d %d", &N, &M);
	for (int i = 0; i < N; ++i) {
		scanf("%s", word);
		insertNode(word);	
	}

	Node** node;
	for (int i = 0; i < M; ++i) {
		scanf("%s", word);
		if(findNode(word, node)) result++;	
	}
	printf("%d", result);

	return 0;
}

