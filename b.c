#if 0

#include <stdio.h>
#include <stdint.h>
#include <complex.h>
#include <string.h>

#define b2bin_pat "%c%c%c%c%c%c%c%c"
#define b2bin(byte) \
		((byte) & 0x80? '1' : '0'), \
		((byte) & 0x40? '1' : '0'), \
		((byte) & 0x20? '1' : '0'), \
		((byte) & 0x10? '1' : '0'), \
		((byte) & 0x08? '1' : '0'), \
		((byte) & 0x04? '1' : '0'), \
		((byte) & 0x02? '1' : '0'), \
		((byte) & 0x01? '1' : '0')

const char* btobin(int x) {
	static char b[9];
	b[0] = '\0';
	int z;
	for(z=128;z>0;z>>=1) {
		strcat(b, ((x&z)==z)? "1":"0");
	}
	return b;
}

const char *bit_rep[16] = {
		[ 0] = "0000", [ 1] = "0001",[ 2] = "0010",[ 3] = "0011",
		[ 4] = "0100", [ 5] = "0101",[ 6] = "0110",[ 7] = "0111",
		[ 8] = "1000", [ 9] = "1001",[10] = "1010",[11] = "1011",
		[12] = "1100", [13] = "1101",[14] = "1110",[15] = "1111" };

const char* btostr(uint8_t byte) {
	static char s[9];
	s[8]='\0';
	strcat(s, bit_rep[byte>>4]);
	strcat(s+4, bit_rep[byte&0x0F]);
	return s;
}

const char* itostr(char* s, uint32_t i) {
	s[32]='\0';
	char* bs;
	uint32_t t;

	t = (i>>24)&0x000F;
	bs = btostr(t);
	strcpy(s,  bs);
	printf("0:%u=%s\n", t, bs);

	t = (i>>16)&0x000F;
	bs = btostr(t);
	strcpy(s+ 8, bs);
	printf("1:%u=%s\n", t, bs);

	t = (i>> 8)&0x000F;
	bs = btostr(t);
	strcpy(s+16, bs);
	printf("2:%u=%s\n", t, bs);

	t = i&0x000F;
	bs = btostr(t);
	strcpy(s+24, bs);
	printf("3:%u=%s\n", t, bs);

	printf("%u:%s\n", i, s);
	return s;
}

int main() {
	uint8_t a = 5, b = 9, c= 255;
	printf("a=%u, b=%u, c=%u\n", a,b,c);

	double complex z1 = 1.0 + 3.0*I;
	printf("z1 = %.02f + %.02fi\n", crealf(z1), cimagf(z1));

	int allmask;
	allmask = (1<<3)-1;
	printf("allmast((1<<3)-1)=%d\n", allmask);

	printf("b=%d(0b"b2bin_pat")\n", b, b2bin(b));
	printf("allmast=%d(0b"b2bin_pat")\n", allmask, b2bin(allmask));

	{
		int c = 0;
		printf("%d=0b%s\n", c=5, btobin(5));	
	}

	{
		int c = 1;
		printf("%d=0b%s\n", c=5, btostr(c));
	}
	{
			uint32_t t;
			printf("0:%u\n", t = (UINT32_MAX)     & 0x000f);
			printf("t:%u=%s\n", t, btostr(t));

			printf("1:%u\n", t = (UINT32_MAX>> 8) & 0x000f);
			printf("t:%u=%s\n", t, btostr(t));

			printf("2:%u\n", t = (UINT32_MAX>>16) & 0x000f);
			printf("t:%u=%s\n", t, btostr(t));

			printf("3:%u\n", t = (UINT32_MAX>>24) & 0x000f);
			printf("t:%u=%s\n", t, btostr(t));
	}
	if(1)
	{
		uint32_t c = UINT32_MAX;
		char s[33];
		s[32]='\0';
		printf("%u=0b%s\n", c=UINT32_MAX, itostr(s, c));
	}

//	double imaginary z = 3*I;
//	z = 1/z;
//	printf("1/(3.0i) = %+.lfi\n", cimag(z));
	return 0;
}

#endif

#if 1

#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>

struct test_struct
{
    int val;
    struct test_struct *next;
};

struct test_struct *head = NULL;
struct test_struct *curr = NULL;

struct test_struct* create_list(int val)
{
    printf("\n creating list with headnode as [%d]\n",val);
    struct test_struct *ptr = (struct test_struct*)malloc(sizeof(struct test_struct));
    if(NULL == ptr)
    {
        printf("\n Node creation failed \n");
        return NULL;
    }
    ptr->val = val;
    ptr->next = NULL;

    head = curr = ptr;
    return ptr;
}

struct test_struct* add_to_list(int val, bool add_to_end)
{
    if(NULL == head)
    {
        return (create_list(val));
    }

    if(add_to_end)
        printf("\n Adding node to end of list with value [%d]\n",val);
    else
        printf("\n Adding node to beginning of list with value [%d]\n",val);

    struct test_struct *ptr = (struct test_struct*)malloc(sizeof(struct test_struct));
    if(NULL == ptr)
    {
        printf("\n Node creation failed \n");
        return NULL;
    }
    ptr->val = val;
    ptr->next = NULL;

    if(add_to_end)
    {
        curr->next = ptr;
        curr = ptr;
    }
    else
    {
        ptr->next = head;
        head = ptr;
    }
    return ptr;
}

struct test_struct* search_in_list(int val, struct test_struct **prev)
{
    struct test_struct *ptr = head;
    struct test_struct *tmp = NULL;
    bool found = false;

    printf("\n Searching the list for value [%d] \n",val);

    while(ptr != NULL)
    {
        if(ptr->val == val)
        {
            found = true;
            break;
        }
        else
        {
            tmp = ptr;
            ptr = ptr->next;
        }
    }

    if(true == found)
    {
        if(prev)
            *prev = tmp;
        return ptr;
    }
    else
    {
        return NULL;
    }
}

int delete_from_list(int val)
{
    struct test_struct *prev = NULL;
    struct test_struct *del = NULL;

    printf("\n Deleting value [%d] from list\n",val);

    del = search_in_list(val,&prev);
    if(del == NULL)
    {
        return -1;
    }
    else
    {
        if(prev != NULL)
            prev->next = del->next;

        if(del == curr)
        {
            curr = prev;
        }
        else if(del == head)
        {
            head = del->next;
        }
    }

    free(del);
    del = NULL;

    return 0;
}

void print_list(void)
{
    struct test_struct *ptr = head;

    printf("\n -------Printing list Start------- \n");
    while(ptr != NULL)
    {
        printf("\n [%d] \n",ptr->val);
        ptr = ptr->next;
    }
    printf("\n -------Printing list End------- \n");

    return;
}

int main(void)
{
    int i = 0, ret = 0;
    struct test_struct *ptr = NULL;

    print_list();

    for(i = 5; i<10; i++)
        add_to_list(i,true);

    print_list();

    for(i = 4; i>0; i--)
        add_to_list(i,false);

    print_list();

    for(i = 1; i<10; i += 4)
    {
        ptr = search_in_list(i, NULL);
        if(NULL == ptr)
        {
            printf("\n Search [val = %d] failed, no such element found\n",i);
        }
        else
        {
            printf("\n Search passed [val = %d]\n",ptr->val);
        }

        print_list();

        ret = delete_from_list(i);
        if(ret != 0)
        {
            printf("\n delete [val = %d] failed, no such element found\n",i);
        }
        else
        {
            printf("\n delete [val = %d]  passed \n",i);
        }

        print_list();
    }

    return 0;
}

#endif
