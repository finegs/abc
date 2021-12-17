#include <stdio.h>


int main() {


	const char* a = "ab0";
	printf("%c\n", (*((a++)+1))+2);
	printf("%s\n", a);
	return 0;
}
