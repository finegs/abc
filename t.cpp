#include <cstring>
#include <iostream>
#include <fstream>
using namespace std;
long x,i;
long double a;
char s[1000000], *p, b[1000000], d[]=" ";
int main(int argc, char* argv[])
{ 

	ifstream f(argv[2]);
	ofstream g(argv[1]);
	do
    { f.get(s,1000000);
      f.get();
      p=strchr(s,' ');
      while (p)
      { i=p-s;
        strncpy(b,s,1000000);
        b[i]='\n';
        a=atof(b);
        g<<a<<' ';
        strcpy(s,s+i+1);
        p=strchr(s,' ');
      }
      g<<atof(s)<<' '<<endl;
    } while (!f.eof());
	return 0;
}
