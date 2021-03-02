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

#define swap(a,b) {int t = a; (a) = (b); (b) = t; }

const char* timestamp();

void heap_push(int arr[], int pos) {
	if(pos<=1||arr[pos/2]>arr[pos]) return;
	swap(arr[pos/2], arr[pos]);
	heap_push(arr, pos/2);
}

void heap_pop(int arr[], int n, int pos) {
	int np = pos*2;
	if(np<n&&arr[np]<arr[np+1]) np++;
	if(np>n||arr[pos]>=arr[np]) return;
	swap(arr[pos], arr[np]);
	heap_pop(arr, n, np);
}

void print(int arr[], int arr_len) {
	fprintf(stdout, "%s Array : [", timestamp());
	for(int i = 1;i<=arr_len;i++) { printf("%d ", arr[i]); }
	//std::for_each_n(arr+1, arr_len, [](int a) { printf("%d ", a); });
	//std::for_each(arr, arr+arr_len, [](int a) { printf("%d ", a); });
	fprintf(stdout, "]"); fflush(stdout);
}

const char* timestamp() {
	time_t rawTime;
	struct tm* pTimeInfo;

	rawTime = time(NULL);
	pTimeInfo = localtime(&rawTime);

	timeval curTime;
	gettimeofday(&curTime, NULL);
	int milli = curTime.tv_usec / 1000;
	
	static char tmstmp_buf [25];
	memset(tmstmp_buf, 0, 25);
//	strftime(tmstmp_buf, 80, "%Y-%m-%d %H:%M:%S", localtime(&curTime.tv_sec));
	strftime(tmstmp_buf, 20, "%Y-%m-%d %H:%M:%S", pTimeInfo);
	
	sprintf(tmstmp_buf+19, ".%03d", milli);
//	printf("current time: %s \n", currentTime);
	return tmstmp_buf;
}

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
	for(int i = 1;i <= arr_len;i++) { arr[i] = rand(); heap_push(arr, i);};	

	fprintf(stdout, "%s arr = rand() && heap_push(%d) is done.\n", timestamp(), arr_len+1);
	if(verbose) {print(arr, arr_len); fprintf(stdout, "\n"); }
	for(int i = arr_len;i>1;i--) { swap(arr[1], arr[i]); heap_pop(arr, i-1, 1); }

	fprintf(stdout, "%s swap & heap_pop(%d) is doen.\n", timestamp(), arr_len+1);
	if(verbose) {print(arr, arr_len); fprintf(stdout, "\n"); }

	free(arr);

	return 0;
}
