#include <cstdio>
#include <iostream>

template <typename Test> 
void test (Test t)
{
    const clock_t begin = clock();
    t();
    const clock_t end = clock();
    std::cout << (end-begin)/double(CLOCKS_PER_SEC) << " sec\n";
}

void scanf_() {
    char x,y,c;
    unsigned dependency_var = 0;
    
    while (!feof (stdin)) {
        scanf ("%c%c%c", &x, &y, &c);
        dependency_var += x + y + c;
    }
    
    std::cout << dependency_var << '\n';
}

void unsynced() {
    std::ios_base::sync_with_stdio (false);
    char x,y,c;
    unsigned dependency_var = 0;
    while (std::cin) {
        std::cin >> x >> y >> c;
        dependency_var += x + y + c; 
    }
    std::cout << dependency_var << '\n';
}

void usage() { std::cout << "one of (scanf|unsynced), pls\n"; }

int main (int argc, char *argv[]) {
    if (argc < 2) { usage(); return 1; }
    
    if (std::string(argv[1]) == "scanf") test (scanf_);
    else if (std::string(argv[1]) == "unsynced") test (unsynced);
    else { usage(); return 1; }

    return 0;
}
