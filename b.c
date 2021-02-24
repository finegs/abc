//Program.c
#include "common.h"
#include "Book.h"
typedef void *Element;
typedef int (*Compare)(Element , Element);               

void quick_sort(Element *base, int n, Compare compare)
{
    Element temp;//��ȯ�� ���� �ӽ� ����
    int pivot = 0; //�ǹ��� ��ġ�� ����ϱ� ���� ����
    int big=0, small=0; //�ǹ����� ū ���� ���� ���� ��ġ�� ã�� ���� ����
    if(n<=1)
    {
        return; 
    }//    ����(n<=1)   ����
    //�ǹ��� �����Ѵ�.    
    if(compare(base[0],base[n-1])>0) 
    {
        if(compare(base[0],base[n/2])>0) //base[0]�� ���� ū ��, �� ������ ������ ���� 0��° ���Ұ� �߰� ��
        {
            if(compare(base[n/2],base[n-1])>0) //�� �߿� ū ���� �߰� ��
            {
                pivot = n/2;
            }
            else
            {
                pivot = n-1;
            }
        }		
    }
    else //base[n-1]�� base[0]���� ũ�ų� ����
    {
        if(compare(base[n/2],base[n-1])>0)
        {			
            pivot = n-1; //n-1��° ���Ұ� �߰� ��
        }
        else//n-1��° ���Ұ� ���� ū ��
        {
            if(compare(base[n/2],base[0])>0)//�� ������ ������ ���� 0��° ���Ұ� �߰� ��
            {
                pivot = n/2;//n/2�� �߰� ��
            }
        }
    }

    //�ǹ��� �� ���� ��Ҹ� ��ȯ�Ѵ�.
    temp = base[pivot];
    base[pivot] = base[0];
    base[0] = temp;

    big=0;//big:=0
    small = n;//small:=n
    while(big<small)//�ݺ�(big<small)
    {
        for(big++; big<n; big++)//        �ݺ�(big:=big+1; big<n; big:=big+1)
        {
            if(compare(base[0],base[big])<0)//            ����(compare(base[0],base[big])<0)
            {
                break;//                ���� Ż��
            }
        }
        for(small--; small>0; small--)//        �ݺ�(small:small-1;small>0;small:small-1)
        {
            if(compare(base[0],base[small])>0)//            ����(compare(base[0],base[small])>0)
            {
                break;//                ���� Ż��
            }			
        }
        if(big<small)//        ����(big<small)
        {
            //            ��ȯ(base [big], base [small])
            temp = base[big];
            base[big] = base[small];
            base[small] = temp;
        }
    }
    
    //��ȯ(base [0], base [small])
    temp = base[0];
    base[0] = base[small];
    base[small] = temp;
    quick_sort(base,small,compare);//�� ����(base,small, compare)
    quick_sort(base+big,n-big,compare);//�� ����(base+big, n-big, compare)
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
        books[i] = New_Book(title,author,rand());//���� �׽�Ʈ ��
        //books[i] = New_Book(title,author,i+1); //������ �׽�Ʈ ��
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
    printf("--------�����-------\n");
    ListBook(10);
    quick_sort((Element *)books,10,CompareByNum);
    printf("--------��ȣ��-------\n");
    ListBook(10);
}
void Simulation2()
{
    clock_t st,et;
    st = clock();
    quick_sort(books,MAX_BOOK/10,CompareByNum);
    et=clock();
    printf("%d�� ���Ŀ� �ɸ� �ð�:%d\n",MAX_BOOK/10,et-st);
    st = clock();
    quick_sort(books,MAX_BOOK,CompareByNum);
    et=clock();	
    printf("%d�� ���Ŀ� �ɸ� �ð�:%d\n",MAX_BOOK,et-st);
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
