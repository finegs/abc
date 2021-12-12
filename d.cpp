#include <stdio.h>
#include <malloc.h>

char* mstrcpy(char dst[], const char src[]) {
    int i = 0;
    while((dst[i] = src[i]) != '\0') ++i; 
    return dst;
}

unsigned long mstrcmp(const char str1[], const char str2[]) {
    int i = 0;
    while(str1[i] != '\0' && str1[i] == str2[i]) ++i;
    return str1[i] - str2[i];
}

unsigned int mstrhash(const char str[], unsigned int TABLE_SIZE) {
    unsigned int h = 5381;
    char c = 0;
    while((c = *str++) != '\0') {
        h = (((h<<5) + h) + c) % TABLE_SIZE;
    }
    return h % TABLE_SIZE;
}

int main(int argc, char* argv[]) {
//    char* src = (char*)malloc(256);
    char* src = new char[256+1]{0};
    if(argc > 0)
        mstrcpy(src, argv[1]);
    else 
        mstrcpy(src, "abc");
    
    char dst[256]{0};
    mstrcpy(dst, src);
    printf("mstrcmp(%s, %s=mstrcpy(%s))=%s\n", src, dst, src, (mstrcmp(src, dst) == 0 ? "true" : "false"));
    printf("mstrhash(%s)=%u\n", src, mstrhash(src, 1000000));

    delete[] src;

    return 0;
}
