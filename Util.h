#pragma once

#include <stdio.h>
#include <stdint.h>
#include <malloc.h>

#ifdef __cplusplus
extern "C" {
#endif

unsigned long  strhash(const char* str, int mod = 1024*1024);
uint32_t hash(void* buf, size_t len, uint32_t* hash);
unsigned int hash_void(const void *key);
void mmemset(void* p, int v, size_t len);
int prime_max(int n);

#ifdef __cplusplus
}
#endif

