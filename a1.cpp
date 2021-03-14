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

//#define swap(a,b) {int* t = a; (a) = (b); (b) = t; }
/*#define swap(x,y) do \
{ unsigned char swap_temp[sizeof(x) == sizeof(y) ? sizeof(x) : -1]; \
	memcpy(swap_temp, &y, sizeof(x)); \
	memcpy(&y, &x, sizeof(x)); \
	memcpy(&x, swap_temp, sizeof(x)); \
} while(0)
*/
template<typename T>
inline void swap(T &a, T &b) { T tmp = a; a = b; b = tmp; }

void swap2(void** pa, void** pb);
static char timestamp_buf[25];
const char* timestamp(char* buf = timestamp_buf);
int comp_int(void* a, void* b);
void heap_push(void* arr, int pos, int (*comp)(void *a, void*b));
void heap_pop(void* arr, int n, int pos, int (*comp)(void *a, void*b));
void print(int arr[], int arr_len);
const char* timestamp(char* buf);

void swap_void(void* a, void* b, size_t size);

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
	for(int i = 1;i <= arr_len;i++) { arr[i] = rand(); heap_push(arr, i, comp_int); }	

	fprintf(stdout, "%s arr = rand() && heap_push(%d) is done.\n", timestamp(), arr_len+1);
	if(verbose) {print(arr, arr_len); fprintf(stdout, "\n"); }
	for(int i = arr_len;i>1;i--) { 
		swap2((void**)&arr+1, (void**)&arr+i); 
		heap_pop(arr, i-1, 1, comp_int); 
	}

	fprintf(stdout, "%s swap & heap_pop(%d) is done.\n", timestamp(), arr_len+1);
	if(verbose) {print(arr, arr_len); fprintf(stdout, "\n"); }

	free(arr);

	cout << "Good " << endl;

	return 0;
}


void heap_push(void* arr, int pos, int (*comp)(void* a, void* b)) {
	if(pos<=1||comp((int*)arr+pos/2, (int*)arr+pos)>0) return;
	swap2(&arr+pos/2, &arr+pos);
	heap_push(arr, pos/2, comp);
}

void heap_pop(void* arr, int n, int pos, int (*comp)(void* a, void* b)) {
	int np = pos*2;
	if(np<n&&comp((int*)arr+np,(int*)arr+np+1)<0) np++;
	if(np>n||comp((int*)arr+pos,(int*)arr+np)<=0) return;
	swap2(&arr+pos, &arr+np);
	heap_pop(arr, n, np, comp);
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

void swap2(void** pa, void** pb) {
	void *pc = *pa;
	*pa = *pb;
	*pb = pc;
}
