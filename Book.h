
// Book.h

#pragma once

#define MAX_TIT_LEN 200
#define MAX_AUT_LEN 20

typedef struct _Book Book;
struct _Book {
	char title[MAX_TIT_LEN];
	char author[MAX_AUT_LEN];
	int num;
};

// default argument value is not supported in c (but c++ is OK)
//void Book_Book(Book *book,const char *title,const char *author,int no = 0);
void Book_Book(Book *book,const char *title,const char *author,int no);

// default argument value is not supported in c (but c++ is OK)
//void Book_Book(Book *book,const char *title,const char *author,int no = 0);
void Book_Book(Book *book,const char *title,const char *author,int no);
Book* Book_New(const char *title, const char* author, int no);
void Book_Delete(Book* book);
void Book_View(Book* book);
int Book_CompareTitle(Book *book,const char *title);
int Book_CompareAuthor(Book *book, const char *author);
int Book_CompareNum(Book *book,int num);
