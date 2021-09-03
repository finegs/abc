
#include <stdio.h>

#define MAX_N 			5
#define MAX_NAME_LEN 	7
#define MAX_TAG_LEN 	4

#define MAX_TYPE_NUM	6

#define MAX_TABLE 		50021
#define MAX_HASH_NODE	300005

#define MAX_SECTION 	105
#define CASE_NAME 		0
#define CASE_TYPE 		1


void mstrcpy(char dst[], const char src[]) {
	int c = 0;
	while((dst[c] = src[c]) != '\0') ++c;
}

int mstrcmp(const char a[], const char b[]) {
	int c = 0;
	while(a[c] != '\0' && a[c] == b[c]) ++c;
	return a[c] - b[c];
}

unsigned long hash(const char* s) {
	unsigned long hash = 5381;
	int c;

	while((c = *s++)) {
		hash = (((hash<<5) + hash) + c) % MAX_TABLE;
	}
	return hash % MAX_TABLE;
}


typedef struct Book_ Book;
typedef struct Obj_ Obj;

struct Book_ {
	char mName[MAX_NAME_LEN];
	int mTypeNum;
	char mTypes[MAX_TYPE_NUM][MAX_TAG_LEN];
	int mSection;
	Book *n;
	Book *p;

	Obj*	typeNode[MAX_TYPE_NUM];
	bool	isCounted;
} book_arr[MAX_TABLE];
int arr_idx = 0;


struct Obj_ {
	Book* addr;
	Obj* n;
	Obj* p;
	char str[MAX_NAME_LEN];
} obj_arr[MAX_HASH_NODE];

int obj_arr_idx =0;


Book* mmalloc() {
	return &book_arr[arr_idx++];
}

Obj* hashalloc() {
	return &obj_arr[obj_arr_idx++];
}

struct Section {
	Book h, t;
} s[MAX_SECTION];

struct tb {
	Obj h, t;
};


tb nameHash[MAX_TABLE];
tb typeHash[MAX_TABLE];

int M;


void init(int m) {


	arr_idx = 0;
	obj_arr_idx = 0;


	M = m;

	for(register int i = 1;i < M+1;++i) {
		(s[i].h).n = &(s[i].t);
		(s[i].t).p = &(s[i].h);
	}


	for (register int i = 0;i < MAX_TABLE;++i) {
		(nameHash[i].h).n = &(nameHash[i].t);
		(nameHash[i].t).p = &(nameHash[i].h);

		(typeHash[i].h).n = &(typeHash[i].t);
		(typeHash[i].t).p = &(typeHash[i].h);
	}
}

void addHash(const char* src, int Case, Book* ptr, int type_idx) {
	Obj* tmp = hashalloc();
	tmp->addr = ptr;
	mstrcpy(tmp->str, src);

	int key  = hash(src);

	if(Case == CASE_NAME) {
		tmp->p = &(nameHash[key].h);
		tmp->n = nameHash[key].h.n;

		nameHash[key].h.n->p = tmp;
		nameHash[key].h.n = tmp;
	}
	else {
		tmp->p = &(typeHash[key].h);
		tmp->n = typeHash[key].h.n;

		typeHash[key].h.n->p = tmp;
		typeHash[key].h.n = tmp;

		ptr->typeNode[type_idx]  =tmp;
	}
}


void add(char mName[MAX_NAME_LEN], int mTypeNum, char mTypes[MAX_N][MAX_TAG_LEN], int mSection) {

	Book* tmp = mmalloc();
	mstrcpy(tmp->mName, mName);
	tmp->mSection = mSection;
	tmp->mTypeNum = mTypeNum;

	for(register int i = 0;i<mTypeNum;++i) {
		mstrcpy(tmp->mTypes[i], mTypes[i]);
	}

	tmp->p = &(s[mSection].h);
	tmp->n = s[mSection].h.n;

	s[mSection].h.n->p = tmp;
	s[mSection].h.n= tmp;

	addHash(mName, CASE_NAME, tmp, -1);


	for(register int i = 0;i<mTypeNum;++i) {
		addHash(mTypes[i], CASE_TYPE, tmp, i);
	}
}

int moveType(char mType[MAX_TAG_LEN], int mFrom, int mTo) {
	int ret = 0;

	int key = hash(mType);
	for(register Obj* cur = (typeHash[key].h).n; cur != nullptr;cur = cur->n) {
		if(mstrcmp(cur->str, mType) == 0) {
			if(cur->addr->mSection == mFrom) {
				Book* target = cur->addr;
				target->mSection = mTo;

				target->p = &s[mTo].h;
				target->n = s[mTo].h.n;

				s[mTo].h.n->p = target;
				s[mTo].h.n = target;

				ret++;
			}
		}
	}
	return ret;
}




