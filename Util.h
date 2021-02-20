#pragma once

#include <stdio.h>
#include <stdint.h>
#include <malloc.h>

int  strhash(const char* str);
uint32_t hash(void* buf, size_t len, uint32_t* hash);
void mmemset(void* p, int v, size_t len);
int prime_max(int n);

