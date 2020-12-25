//Program.c
#include "common.h"
#include "Book.h"
typedef void *Element;
typedef int (*Compare)(Element , Element);               

void quick_sort(Element *base, int n, Compare compare)
{
    Element temp;//교환을 위한 임시 변수
    int pivot = 0; //피벗의 위치를 기억하기 위한 변수
    int big=0, small=0; //피벗보다 큰 값과 작은 값의 위치를 찾기 위한 변수
    if(n<=1)
    {
        return; 
    }//    조건(n<=1)   종료
    //피벗을 선택한다.    
    if(compare(base[0],base[n-1])>0) 
    {
        if(compare(base[0],base[n/2])>0) //base[0]이 제일 큰 값, 이 조건인 거짓일 때는 0번째 원소가 중간 값
        {
            if(compare(base[n/2],base[n-1])>0) //둘 중에 큰 값이 중간 값
            {
                pivot = n/2;
            }
            else
            {
                pivot = n-1;
            }
        }		
    }
    else //base[n-1]이 base[0]보다 크거나 같음
    {
        if(compare(base[n/2],base[n-1])>0)
        {			
            pivot = n-1; //n-1번째 원소가 중간 값
        }
        else//n-1번째 원소가 제일 큰 값
        {
            if(compare(base[n/2],base[0])>0)//이 조건인 거짓일 때는 0번째 원소가 중간 값
            {
                pivot = n/2;//n/2가 중간 값
            }
        }
    }

    //피벗과 맨 앞의 요소를 교환한다.
    temp = base[pivot];
    base[pivot] = base[0];
    base[0] = temp;

    big=0;//big:=0
    small = n;//small:=n
    while(big<small)//반복(big<small)
    {
        for(big++; big<n; big++)//        반복(big:=big+1; big<n; big:=big+1)
        {
            if(compare(base[0],base[big])<0)//            조건(compare(base[0],base[big])<0)
            {
                break;//                루프 탈출
            }
        }
        for(small--; small>0; small--)//        반복(small:small-1;small>0;small:small-1)
        {
            if(compare(base[0],base[small])>0)//            조건(compare(base[0],base[small])>0)
            {
                break;//                루프 탈출
            }			
        }
        if(big<small)//        조건(big<small)
        {
            //            교환(base [big], base [small])
            temp = base[big];
            base[big] = base[small];
            base[small] = temp;
        }
    }
    
    //교환(base [0], base [small])
    temp = base[0];
    base[0] = base[small];
    base[small] = temp;
    quick_sort(base,small,compare);//퀵 정렬(base,small, compare)
    quick_sort(base+big,n-big,compare);//퀵 정렬(base+big, n-big, compare)
}
#define MAX_BOOK	4000
Book *books[MAX_BOOK]={0};
void SimulationInit()
{
    char title[MAX_TIT_LEN+1]="";
    char author[MAX_AUT_LEN+1]="";
    int i = 0;
    for(i=0; i<MAX_BOOK; ++i)
    {
        sprintf(title,"%010d",rand());
        sprintf(author,"%010d",rand());
        books[i] = New_Book(title,author,rand());//랜덤 테스트 용
        //books[i] = New_Book(title,author,i+1); //순차적 테스트 용
    }
}
int CompareByTitle(void *b1,void *b2)
{
    Book *book1=(Book *)b1;
    Book *book2=(Book *)b2;
    return Book_CompareTitle(book1,book2->title);
}
int CompareByNum(void *b1,void *b2)
{
    Book *book1=(Book *)b1;
    Book *book2=(Book *)b2;
    return Book_CompareNum(book1,book2->num);
}

void ListBook(int n)
{
    int i = 0;
    for(i=0; i<n; ++i)
    {
        Book_View(books[i]);
    }
}
void Simulation1()
{
    quick_sort((Element *)books,10,CompareByTitle);
    printf("--------제목순-------\n");
    ListBook(10);
    quick_sort((Element *)books,10,CompareByNum);
    printf("--------번호순-------\n");
    ListBook(10);
}
void Simulation2()
{
    clock_t st,et;
    st = clock();
    quick_sort(books,MAX_BOOK/10,CompareByNum);
    et=clock();
    printf("%d개 정렬에 걸린 시간:%d\n",MAX_BOOK/10,et-st);
    st = clock();
    quick_sort(books,MAX_BOOK,CompareByNum);
    et=clock();	
    printf("%d개 정렬에 걸린 시간:%d\n",MAX_BOOK,et-st);
}
void SimulationClear()
{
    int i = 0;
    for(i=0; i<MAX_BOOK; ++i)
    {
        Delete_Book(books[i]);
    }
}
int main()
{
    SimulationInit();
    Simulation1();
    Simulation2();
    SimulationClear();
    return 0;
}
