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


int main(int argc, char* argv[]) {
    char* src = (char*)malloc(256);
    if(argc > 0)
        mstrcpy(src, argv[1]);
    else 
        mstrcpy(src, "abc");
    
    char dst[256];
    mstrcpy(dst, src);
    printf("mstrcmp(%s, %s=mstrcpy(%s))=%s\n", src, dst, src, (mstrcmp(src, dst) == 0 ? "true" : "false"));

    delete[] src;

    return 0;
}