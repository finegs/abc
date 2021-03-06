#include "Util.h"

unsigned long strhash(const char* str, unsigned long mod) {
	unsigned long h = 5731;
	char c;
	char* ss = (char*)str;
	while((c = *ss++)) {
		h+=((h<<5) + h) + c;
		if(mod>0)h%=mod;
	}
	return h;
}

uint32_t hash(void *buf, size_t len, uint32_t* hval) {
	uint32_t PRIME_32 = 0x01000193;
	unsigned char* bp = (unsigned char*)buf;
	unsigned char* be = bp+len;

	while(bp<be) {
#if defined(HAHS_SIMPLE) || 1
			*hval *= PRIME_32;
#else
			*hval += (*hval<<1)+(*hval<<4)+(*hval<<7)+(*hval<<8)+(*hval<<24);
#endif
			*hval ^= *bp++;
	}
	return *hval;
}

unsigned int hash(const char* key) {
	const char *p;
	unsigned int val = 5371;
	p =(char*)key;
	while(*p!='\0') {
		unsigned int tmp;
		val = ((val<<5) * val)+(*p);
//		if((tmp=(val & 0xf0000000))) {
//			val = val ^ (tmp>>24);
//			val = val ^ tmp;
//		}
	}
	return val;
}
unsigned int hash_void(const void *key) {
	const char *p;
	unsigned int val;
	val = 0x01000193;

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


void* memset(void* p, int v, size_t len) {
	char* pp = (char*)p;
	while(len--) { *pp++ = v; }
	return p;
}

int prime_max(int n) {
    // Create a boolean array 
    // "prime[0..n]" and initialize
    // all entries it as true. 
    // A value in prime[i] will
    // finally be false if i is 
    // Not a prime, else true.
    bool prime[n + 1];
    memset(prime, true, sizeof(prime));
 
    for (int p = 2; p * p <= n; p++)
    {
        // If prime[p] is not changed, 
        // then it is a prime
        if (prime[p] == true) 
        {
            // Update all multiples 
            // of p greater than or
            // equal to the square of it
            // numbers which are multiple 
            // of p and are less than p^2 
            // are already been marked.
            for (int i = p * p; i <= n; i += p) prime[i] = false;
        }
    }
 
    // Print all prime numbers
	int pn;
    for (int p = 2; p <= n; p++) { if (prime[p]) pn = p; }
	return pn;
}

int strncmp(const char* a, const char* b, size_t len) {
	while(len-->0) {
		if(*a!=*b) return (*a>*b? 1 : -1);
		a++;
		b++;
	}
	return 0;
}

int strcmp(const char* a, const char* b) {
	return strncmp(a, b, strlen(a));
}

char* strncpy(char* dst, const char* src, size_t len) {
	char* dp = dst;
	char* sp = (char*)src;
	while(len-->0) *dst++ = *sp++;
	return dp;
}


size_t strlen(const char* p) {
	size_t l = 0;
	char* pp = (char*)p;
	while(*pp++) { l++; }
	return l;
}

char* strstr(const char* str1, const char* str2) {
	char *cp = (char *)str1;
	char *s1, *s2;
	if ( !*str2 )
		return((char *)str1);
 
	while (*cp)
	{
		s1 = cp;
		s2 = (char *) str2;
		while ( *s2 && !(*s1 - *s2) )
			s1++, s2++;
		if (!*s2)
			return(cp);
		cp++;
	}
 
	return(NULL);
}

#if 0
size_t strncpy(char* p, const char* s, size_t s_len) {
	size_t ll = s_len;
	while(ll-->0) { *(p+ll-1) = *(s+ll-1); }
	return s_len;
}
#endif

#ifdef __cpluscplus
}
#endif

#if 0

char* itoa(int num, char* buffer, int base) {
    int curr = 0;
 
    if (num == 0) {
        // Base case
        buffer[curr++] = '0';
        buffer[curr] = '\0';
        return buffer;
    }
 
    int num_digits = 0;
 
    if (num < 0) {
        if (base == 10) {
            num_digits ++;
            buffer[curr] = '-';
            curr ++;
            // Make it positive and finally add the minus sign
            num *= -1;
        }
        else
            // Unsupported base. Return NULL
            return NULL;
    }
 
    num_digits += (int)floor(log(num) / log(base)) + 1;
 
    // Go through the digits one by one
    // from left to right
    while (curr < num_digits) {
        // Get the base value. For example, 10^2 = 1000, for the third digit
        int base_val = (int) pow(base, num_digits-1-curr);
 
        // Get the numerical value
        int num_val = num / base_val;
 
        char value = num_val + '0';
        buffer[curr] = value;
 
        curr ++;
        num -= base_val * num_val;
    }
    buffer[curr] = '\0';
    return buffer;
}

#endif
 
 #if 0
int main() {
    int a = 1234;
    char buffer[256];
    if (itoa(a, buffer, 10) != NULL) {
        printf("Input = %d, base = %d, Buffer = %s\n", a, 10, buffer);
    }
    int b = -231;
    if (itoa(b, buffer, 10) != NULL) {
        printf("Input = %d, base = %d, Buffer = %s\n", b, 10, buffer);
    }
    int c = 10;
    if (itoa(c, buffer, 2) != NULL) {
        printf("Input = %d, base = %d, Buffer = %s\n", c, 2, buffer);
    }
    return 0;
}
#endif