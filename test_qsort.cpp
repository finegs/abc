#include <cstdio>
#include <malloc.h>

void memset(void*p, unsigned char c, size_t len) {
	unsigned char* pp = (unsigned char*)p;
	while(len--) *pp++=c;
}

void memcpy(void*dst,void*src,size_t len) {
	unsigned char* dd = (unsigned char*)dst;
	unsigned char* ss = (unsigned char*)src;
	while(len--) *dd++=*ss++;
}

void swap(void* a, void* b, size_t len, void* t = nullptr) {
	void* tt = nullptr;
	if(!t) { tt = malloc(len); t = tt; }
	memcpy(t, a, len);
	memcpy(a, b, len);
	memcpy(b, t, len);
	if (tt) free(tt);
}

int qsort_partition(void* dataSet, size_t dataSize, int left, int right);
void qsort(void* dataSet, size_t dataSize, int (*dataComparator)(void*a, void*b), int left, int right);
int IntComparator(void* a, void* b);
#if 0
int qsort_partitin(int* dataSet, int left, int right) {
	int first = left;
	int pivot = dataSet[first];

	left++;
	int tt;

	while(left<=right) {
		while(dataSet[left]<=pivot&&left<right) ++left;
		while(dataSet[right]>pivot&&left<=right) --right;
		if(left<right)
			swap(&dataSet[left], &dataSet[right], sizeof(int), &tt);
		else
			break;
	}
	swap(&dataSet[first], &dataSet[right], sizeof(int), &tt);
	return right;
}

void qsort(int* dataSet, int left, int right) {
	if(left<right) {
		int idx = qsort_partitin(dataSet, left, right);
		qsort(dataSet, left, idx-1);
		qsort(dataSet, idx+1, right);
	}
}
#endif

void print(void* dataSet, size_t len, void (*printData)(void* data)) {
	int* sp = (int*)dataSet;
	printf("{length:%d, data:[", len);
	for(size_t i = 0;i<len;i++) {
		printData(sp+i); if(i<len-1) printf(", ");
	}
	printf("]}\n");
}

void printInt(void* data) {
	printf("%d", *((int*)data));
}

int main(int argc, char* argv[]) {
	int* arr = nullptr;
	int arr_len = 0, v;
	scanf("%d", &arr_len);
	arr = (int*)malloc(sizeof(int)*arr_len);
	if(!arr) { fprintf(stderr, "fail to malloc(sizeof(int)*%d)", arr_len); return 1; }
	memset(arr, '\0', arr_len);
	for(int i = 0;i < arr_len;i++) {
		scanf("%d", arr+i);
	}

	printf("Before : "); print(arr, arr_len, printInt);

	qsort((void*)arr, sizeof(int), IntComparator, 0, arr_len-1);

	printf("After : "); print(arr, arr_len, printInt);

	return 0;
}

int IntComparator(void* a, void* b) {
	int* aa  = (int*)a;
	int* bb = (int*)b;
	if(*aa == *bb) return 0;
	return *aa > * bb ? 1 : -1;
}


int qsort_partition(void* dataSet, size_t dataSize, int (*dataComparator)(void* a, void* b), int left, int right) {
	int first = left;
	while(left<=right) {
		while(dataComparator((int*)dataSet+left, (int*)dataSet+first)<0 && left<=right) ++left;
		while(dataComparator((int*)dataSet+right,(int*)dataSet+first)>=0&& left<right) --right; 
		if(left<right) swap((int*)dataSet+left, (int*)dataSet+right, dataSize);
		else break;
	}
	swap((int*)dataSet+first,  (int*)dataSet+right, dataSize);
	return right;
}

void qsort(void* dataSet, size_t dataSize, int (*dataComparator)(void*a,void*b), int left, int right) {
	if(left<right) {
		int idx = qsort_partition(dataSet, dataSize, dataComparator, left, right);
		qsort(dataSet, dataSize, dataComparator, left, idx-1);
		qsort(dataSet, dataSize, dataComparator, idx+1, right);
	}
}
