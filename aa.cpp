
#include <stdio.h>
#include <malloc.h>
#include "List.h"

struct Int {
	int v;
};

int main() {
	List l;	

	l.insert(new Int{10});
	l.insert(new Int{20});
	l.insert(new Int{30});
	l.insert(new Int{40});
	l.insert(new Int{50});
	l.insert(new Int{60});

	int a;

	return 0;
}
