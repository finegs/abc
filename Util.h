#pragma once

#include <stdio.h>
#include <stdint.h>
#include <malloc.h>

#ifdef __cplusplus
extern "C" {
#endif

int  strhash(const char* str, int mod = 1024*1024);
uint32_t hash(void* key, size_t len, uint32_t* hash);
unsigned int hash(const char *key);
void mmemset(void* p ,char v, size_t len);
int prime_max(int n);

#ifdef __cplusplus
}
#endif
