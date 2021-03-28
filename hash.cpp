#include "hash.hpp"

unsigned int hash(const void *key) {
	const char *p;
	unsigned int val;
	val = 0;
	p =(char*)key;
	while(*p!='\0') {
		unsigned int tmp;
		val = (val<<4)+(*p);
		if((tmp=(val & 0xf0000000))) {
			val = val ^ (tmp>>24);
			val = val ^ tmp;
		}
		p++;
	}
	return val % 0xffffffff;
}

