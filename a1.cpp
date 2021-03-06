#include <iostream>
#include <cstring>
#include <string>
#include <random>
#include <ctime>
#include <sys/time.h>
#if defined(WIN32) || defined(_WIN64)
#include <Windows.h>
#else
#include <sys/time.h>
#endif
#include "a1.hpp"

using namespace std;

template<typename T>
inline void swap(T &a, T &b) { T tmp = a; a = b; b = tmp; }

int swapb(void* a, void* b, void* buf, size_t len);
static char timestamp_buf[25];
const char* timestamp(char* buf = timestamp_buf);
int comp_int(void* a, void* b);
void heap_push(void* arr, int pos, int (*comp)(void *a, void*b), void* (*buffer)(), size_t (*bufferlen)());
void heap_pop(void* arr, int n, int pos, int (*comp)(void *a, void*b), void* (*buffer)(), size_t (*bufferlen)());
void print(int arr[], int arr_len);
const char* timestamp(char* buf);
int ib;
void* getbuffer() { return &ib; }
size_t getbufferlen() { return sizeof(int); }

int main(int argc, char* argv[]) {

	fprintf(stdout, "%s %s is started.\n", timestamp(), argv[0]);
	int arr_len = 10;
	int verbose = 0;
	for(int i = 1;i<argc;i++) {
		if((!strcmp("-n", argv[i]) || !strcmp("--num", argv[i])) && i+1<argc) {
			arr_len = atoi(argv[i+1]); i++;
		}
		if(!strcmp("-v", argv[i]) || !strcmp("--verbose", argv[i])) {
			verbose = 1;
		}
	}

	if(arr_len <=0) {
	  arr_len = 10;
	  printf("%s Array Size (Default:%d) ", timestamp(), arr_len); fflush(stdout);
	}

	fprintf(stdout, "%s malloc(%d) is done.\n", timestamp(), arr_len+1);
	int* arr = (int*)malloc(sizeof(int)*(arr_len+1));
	if(!arr) { fprintf(stderr, "%s fail to malloc(%d)", timestamp(), arr_len); return EXIT_FAILURE;}

	srand((unsigned int)time(NULL));
	//std::for_each_n(arr+1, arr_len, [](int &a) { a = rand(); });	
	fprintf(stdout, "%s srand(time(NULL)) is done.\n", timestamp());
	for(int i = 1;i <= arr_len;i++) { arr[i] = rand(); heap_push(arr, i, comp_int, getbuffer, getbufferlen); }	

	fprintf(stdout, "%s arr = rand() && heap_push(%d) is done.\n", timestamp(), arr_len+1);
	if(verbose) {print(arr, arr_len); fprintf(stdout, "\n"); }
	for(int i = arr_len;i>1;i--) { 
		swapb((void**)&arr+1, (void**)&arr+i, &ib, sizeof(int)); 
		heap_pop(arr, i-1, 1, comp_int, getbuffer, getbufferlen); 
	}

	fprintf(stdout, "%s swap & heap_pop(%d) is done.\n", timestamp(), arr_len+1);
	if(verbose) {print(arr, arr_len); fprintf(stdout, "\n"); }

	free(arr);

	cout << "Good " << endl;

	return 0;
}


void heap_push(void* arr, int pos, int (*comp)(void* a, void* b), void* (*buffer)(), size_t (*bufferlen)()) {
	if(pos<=1||comp((int*)arr+pos/2, (int*)arr+pos)>0) return;
	swapb(&arr+pos/2, &arr+pos, buffer(), bufferlen());
	heap_push(arr, pos/2, comp, buffer, bufferlen);
}

void heap_pop(void* arr, int n, int pos, int (*comp)(void* a, void* b),void* (*buffer)(), size_t (*bufferlen)() ) {
	int np = pos*2;
	if(np<n&&comp((int*)arr+np,(int*)arr+np+1)<0) np++;
	if(np>n||comp((int*)arr+pos,(int*)arr+np)<=0) return;
	swapb(&arr+pos, &arr+np, buffer(), bufferlen());
	heap_pop(arr, n, np, comp, buffer, bufferlen);
}

void print(int arr[], int arr_len) {
	fprintf(stdout, "%s Array : [", timestamp());
	for(int i = 1;i<=arr_len;i++) { printf("%d ", arr[i]); }
	fprintf(stdout, "]\n");
}

const char* timestamp(char* buf) {
	time_t rawTime;
	struct tm* pTimeInfo;

	rawTime = time(NULL);
	pTimeInfo = localtime(&rawTime);

	timeval curTime;
	gettimeofday(&curTime, NULL);
	int milli = curTime.tv_usec / 1000;
	
	memset(buf, 0, 25);
//	strftime(buf, 80, "%Y-%m-%d %H:%M:%S", localtime(&curTime.tv_sec));
	strftime(buf, 20, "%Y-%m-%d %H:%M:%S", pTimeInfo);
	
	sprintf(buf+19, ".%03d", milli);
//	printf("current time: %s \n", buf);
	return buf;
}

int comp_int(void* a, void* b) {
	return *(int*)a - *(int*)b;
}

int swapb(void* a, void* b, void* t, size_t len) {
	if(!t) return 1;
	memcpy(t, a, len);
	memcpy(a, b, len);
	memcpy(b, t, len);
	return 0;
}

