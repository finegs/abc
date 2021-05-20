#pragma once

#include <stdio.h>
#include <stdint.h>
#include <malloc.h>

#ifdef __cplusplus
extern "C" {
#endif


uint32_t hash(void* buf, size_t len, uint32_t* hash);
unsigned int hash_void(const void *key);
int prime_max(int n);

void* memset(void* p, int c, size_t len);

size_t strlen(const char* p); 
char* strncpy(char* p, const char* s, size_t s_len); 
unsigned long  strhash(const char* str, unsigned long mod);
int strncmp(const char* a, const char* b, size_t len);
int strcmp(const char* a, const char* b);
char* strstr(const char* str1, const char* str2);


#ifdef __cplusplus
}
#endif

