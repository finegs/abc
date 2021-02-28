#include <iostream>
#include <cstring>
#include <string>
#include <random>
#include <ctime>
#include "a1.hpp"

using namespace std;

#define swap(a,b) {int t = a; (a) = (b); (b) = t; }

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
	fprintf(stdout, "Array : [");
	for(int i = 1;i<=arr_len;i++) { printf("%d ", arr[i]); }
	//std::for_each_n(arr+1, arr_len, [](int a) { printf("%d ", a); });
	//std::for_each(arr, arr+arr_len, [](int a) { printf("%d ", a); });
	fprintf(stdout, "]"); fflush(stdout);
}

int main(int argc, char* argv[]) {

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
	  printf("Array Size (Default:%d) ", arr_len); fflush(stdout);
	}

	int* arr = (int*)malloc(sizeof(int)*(arr_len+1));
	if(!arr) { fprintf(stderr, "fail to malloc(%d)", arr_len); return EXIT_FAILURE;}

	srand((unsigned int)time(NULL));
	//std::for_each_n(arr+1, arr_len, [](int &a) { a = rand(); });	
	for(int i = 1;i <= arr_len;i++) { arr[i] = rand(); heap_push(arr, i);};	
	if(verbose) {print(arr, arr_len); fprintf(stdout, "\n"); }
	for(int i = arr_len;i>1;i--) { swap(arr[1], arr[i]); heap_pop(arr, i-1, 1); }
	if(verbose) {print(arr, arr_len); fprintf(stdout, "\n"); }

	free(arr);
	return 0;
}
