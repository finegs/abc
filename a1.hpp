#pragma once

#include <string>
#include <iostream>

//#define swap(x,y) do \
//{ unsigned char swap_temp[sizeof(x) == sizeof(y) ? sizeof(x) : -1]; \
//	memcpy(swap_temp, &y, sizeof(x)); \
//	memcpy(&y, &x, sizeof(x)); \
//	memcpy(&x, swap_temp, sizeof(x)); \
//} while(0)

typedef struct {
	char name[512];
	char desc[512];
	char sellcnt;
	char no;
}Book;

int f1(int a, int b);
