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

using namespace	std;

#define swap(a,b) { int t = (a); (a) = (b); (b) = t; }

const char* timestamp();

void heap_push(int arr[], int pos) {
	if(pos<=1||arr[pos/2]>arr[pos]) return;
	swap(arr[pos/2], arr[pos]);
	heap_push(arr, pos/2);
}

void heap_pop(int arr[], int n, int pos) {
	int np = pos*2;
	if(np<n&&arr[np]<arr[np+1]) np++;
	if(np>n||arr[pos]>arr[np]) return;
	swap(arr[pos], arr[np]);
	heap_pop(arr, n, np);
}

const char* timestamp() {
	time_t rawtime;
	struct tm* ti;

	rawtime = time(NULL);
	ti = localtime(&rawtime);

	timeval curtime;
	gettimeofday(&curtime, NULL);
	int milli= curtime.tv_usec / 1000;

	static char tmstmp_buf[25];
	memset(tmstmp_buf, 0, 25);
	strftime(tmstmp_buf, 20, "%Y-%m-%d %H:%M:%S", ti);

	sprintf(tmstmp_buf+19, ".%03d", milli);
	return tmstmp_buf;
}


