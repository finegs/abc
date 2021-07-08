#include <stdio.h>
#include <malloc.h>

class Item {
    public:
        Item() : i(0) {}
        explicit Item(const int& i) : i(i) {}

        void print(FILE* fp) {
            fprintf(fp, "{");
            fprintf(fp, "\"i\":\"%d\"", i);
            fprintf(fp, "}");
        }

        void scanf(FILE* fp) {
            fscanf(fp, "%d", &i);
        }

    private:
        int i;
};

int main(int argc, char* argv[]) {

    Item* a = new Item();
    fprintf(stdout, "Item.i:"); fflush(stdout);
    a->scanf(stdin);
    a->print(stdout);
    fprintf(stdout, "\n");
    delete a;

    return 0;
}