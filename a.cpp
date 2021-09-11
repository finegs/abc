#if 1

#include <string.h>

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h> 



// 32-bit version
int hash_fun(unsigned int key, int try_num, int max) {
    return (key + try_num) % max;  // the hash fun returns a number bounded by the number of slots.
}


// this hash table has
// key is int
// value is char buffer
struct key_value_pair {
    int key; // use this field as the key
    char *pValue;  // use this field to store a variable length string
};


struct hash_table {
    int max;
    int number_of_elements;
    struct key_value_pair **elements;  // This is an array of pointers to mystruct objects
};


int hash_insert(struct key_value_pair *data, struct hash_table *hash_table) {

    int try_num, hash;
    int max_number_of_retries = hash_table->max;


    if (hash_table->number_of_elements >= hash_table->max) {
        return 0; // FULL
    }

    for (try_num = 0; try_num < max_number_of_retries; try_num++) {

        hash = hash_fun(data->key, try_num, hash_table->max);

        if (NULL == hash_table->elements[hash]) { // an unallocated slot
            hash_table->elements[hash] = data;
            hash_table->number_of_elements++;
            return RC_OK;
        }
    }
    return RC_ERROR;
}


// returns the corresponding key value pair struct
// If a value is not found, it returns null
//
// 32-bit version
struct key_value_pair *hash_retrieve(unsigned int key, struct hash_table *hash_table) {

    unsigned int try_num, hash;
    unsigned int max_number_of_retries = hash_table->max;

    for (try_num = 0; try_num < max_number_of_retries; try_num++) {

        hash = hash_fun(key, try_num, hash_table->max);

        if (hash_table->elements[hash] == 0) {
            return NULL; // Nothing found
        }

        if (hash_table->elements[hash]->key == key) {
            return hash_table->elements[hash];
        }
    }
    return NULL;
}


// Returns the number of keys in the dictionary
// The list of keys in the dictionary is returned as a parameter.  It will need to be freed afterwards
int keys(struct hash_table *pHashTable, int **ppKeys) {

    int num_keys = 0;

    *ppKeys = (int *) malloc( pHashTable->number_of_elements * sizeof(int) );

    for (int i = 0; i < pHashTable->max; i++) {
        if (NULL != pHashTable->elements[i]) {
            (*ppKeys)[num_keys] = pHashTable->elements[i]->key;
            num_keys++;
        }
    }
    return num_keys;
}

// The dictionary will need to be freed afterwards
int allocate_the_dictionary(struct hash_table *pHashTable) {


    // Allocate the hash table slots
    pHashTable->elements = (struct key_value_pair **) malloc(pHashTable->max * sizeof(struct key_value_pair));  // allocate max number of key_value_pair entries
    for (int i = 0; i < pHashTable->max; i++) {
        pHashTable->elements[i] = NULL;
    }



    // alloc all the slots
    //struct key_value_pair *pa_slot;
    //for (int i = 0; i < pHashTable->max; i++) {
    //  // all that he could see was babylon
    //  pa_slot = (struct key_value_pair *)  malloc(sizeof(struct key_value_pair));
    //  if (NULL == pa_slot) {

    //  }
    //  pHashTable->elements[i] = pa_slot;
    //  pHashTable->elements[i]->key = 0;
    //}

    return RC_OK;
}


// This will make a dictionary entry where
//   o  key is an int
//   o  value is a character buffer
//
// The buffer in the key_value_pair will need to be freed afterwards
int make_dict_entry(int a_key, char * buffer, struct key_value_pair *pMyStruct) {

    // determine the len of the buffer assuming it is a string
    int len = strlen(buffer);

    // alloc the buffer to hold the string
    pMyStruct->pValue = (char *) malloc(len + 1); // add one for the null terminator byte
    if (NULL == pMyStruct->pValue) {
        printf("Failed to allocate the buffer for the dictionary string value.");
        return RC_ERROR;
    }
    strcpy(pMyStruct->pValue, buffer);
    pMyStruct->key = a_key;

    return RC_OK;
}


// Assumes the hash table has already been allocated.
int add_key_val_pair_to_dict(struct hash_table *pHashTable, int key, char *pBuff) {

    int rc;
    struct key_value_pair *pKeyValuePair;

    if (NULL == pHashTable) {
        printf("Hash table is null.\n");
        return RC_ERROR;
    }

    // Allocate the dictionary key value pair struct
    pKeyValuePair = (struct key_value_pair *) malloc(sizeof(struct key_value_pair));
    if (NULL == pKeyValuePair) {
        printf("Failed to allocate key value pair struct.\n");
        return RC_ERROR;
    }


    rc = make_dict_entry(key, pBuff, pKeyValuePair);  // a_hash_table[1221] = "abba"
    if (RC_ERROR == rc) {
        printf("Failed to add buff to key value pair struct.\n");
        return RC_ERROR;
    }


    rc = hash_insert(pKeyValuePair, pHashTable);
    if (RC_ERROR == rc) {
        printf("insert has failed!\n");
        return RC_ERROR;
    }

    return RC_OK;
}


void dump_hash_table(struct hash_table *pHashTable) {

    // Iterate the dictionary by keys
    char * pValue;
    struct key_value_pair *pMyStruct;
    int *pKeyList;
    int num_keys;

    printf("i\tKey\tValue\n");
    printf("-----------------------------\n");
    num_keys = keys(pHashTable, &pKeyList);
    for (int i = 0; i < num_keys; i++) {
        pMyStruct = hash_retrieve(pKeyList[i], pHashTable);
        pValue = pMyStruct->pValue;
        printf("%d\t%d\t%s\n", i, pKeyList[i], pValue);
    }

    // Free the key list
    free(pKeyList);

}

int main(int argc, char *argv[]) {

    int rc;
    int i;


    struct hash_table a_hash_table;
    a_hash_table.max = 20;   // The dictionary can hold at most 20 entries.
    a_hash_table.number_of_elements = 0;  // The intial dictionary has 0 entries.
    allocate_the_dictionary(&a_hash_table);

    rc = add_key_val_pair_to_dict(&a_hash_table, 1221, "abba");
    if (RC_ERROR == rc) {
        printf("insert has failed!\n");
        return RC_ERROR;
    }
    rc = add_key_val_pair_to_dict(&a_hash_table, 2211, "bbaa");
    if (RC_ERROR == rc) {
        printf("insert has failed!\n");
        return RC_ERROR;
    }
    rc = add_key_val_pair_to_dict(&a_hash_table, 1122, "aabb");
    if (RC_ERROR == rc) {
        printf("insert has failed!\n");
        return RC_ERROR;
    }
    rc = add_key_val_pair_to_dict(&a_hash_table, 2112, "baab");
    if (RC_ERROR == rc) {
        printf("insert has failed!\n");
        return RC_ERROR;
    }
    rc = add_key_val_pair_to_dict(&a_hash_table, 1212, "abab");
    if (RC_ERROR == rc) {
        printf("insert has failed!\n");
        return RC_ERROR;
    }
    rc = add_key_val_pair_to_dict(&a_hash_table, 2121, "baba");
    if (RC_ERROR == rc) {
        printf("insert has failed!\n");
        return RC_ERROR;
    }



    // Iterate the dictionary by keys
    dump_hash_table(&a_hash_table);

    // Free the individual slots
    for (i = 0; i < a_hash_table.max; i++) {
        // all that he could see was babylon
        if (NULL != a_hash_table.elements[i]) {
            free(a_hash_table.elements[i]->pValue);  // free the buffer in the struct
            free(a_hash_table.elements[i]);  // free the key_value_pair entry
            a_hash_table.elements[i] = NULL;
        }
    }


    // Free the overall dictionary
    free(a_hash_table.elements);


    _CrtDumpMemoryLeaks();
    return 0;
}
#endif

#if 0

#include <stdio.h>

#define MAP_SIZE 100000

void mstrlen(const char str[]) {
  size_t l = 0;
  int c = 0;
  while(*str++ != '\0') ++l;
  return l;
}

void mstrcpy(char dst[], const char src[]) {
  int i = 0;
  while ((dst[i]=src[i]) != '\0') ++i;
}

int mstrcmp(const char str1[], const char str2[]) {
  int i = 0;
  while(str1[i] != '\0' && str1[i] == str2[i]) ++i;
  return str1[i] - str2[i];
}

size_t mstrhash(const char str[], size_t mod) {
  size_t h = 5381;
  char c = 0;
  while((c = *str++) != '\0') {
    h = (h<<5) +h + c;
    h %= mod;
  }
  return h%mod;
}

struct Node {
  char name[32];
  char point;
  Node* next;
};


struct NodeMap {
  NodeMap() : capacity{MAP_SIZE}, size{0} {
    nodeList = (Node**)malloc(sizeof(Node*)*capacity);
    for (size_t i = 0; i < capacity; i++)
    {
      nodeList[i] = nullptr;
    }
    
  }

  ~NodeMap() {
    for (size_t i = 0; i < capacity; i++)
    {
      if(!nodeList[i]) continue;
      free(nodeList[i]);
    }
    free(nodeList);
    nodeList = nullptr;
    
  }
  void insert(char name[], char point) {
    Node** node;
    if(find(name, node))  {
        *node->point = point;
        return;
    }

    *node = (Node*)malloc(sizeof(Node));
    mstrcpy(*node->name, name);
    *node->point = point;

    size_t h = mstrhash(name, MAP_SIZE);

    Node* prev = nodeList[h];
    if(prev) {
      *node->next = prev->next;
      nodeList[i] = *node;
    }
    else {
      nodeList[h] = *node;
    }

    size++;
  }
  int find(char name[], Node** node) {
    size_t h = mstrhash(name, capacity);
    Node* prev = null;
    Node* cur = nodeList[h];
    while(cur) {
      if(mstrcmp(cur->name, name) == 0) break;
      prev = cur;
      cur = cur->next;
    }

    if(cur) {
      *node = cur;
      return 1;
    }
    else 
      return 0;
  }

  void remove(const char name[], Node** node) {

  }
  Node** nodeList;
  size_t size;
  size_t capacity;
};

int main(int argc, char* argv[]) {

  NodeMap nm;

  printf("1.insert\n");
  for (size_t i = 1; i < argc; i++)
  {
    nm.insert(argv[i], mstrlen(argv[i]));
  }

  printf("2.find\n");
  Node* node;
  for (size_t i = 1; i < argc; i++)
  {
    if(nm.find(argv[i], &node)) 
      printf("find(%s)=true", *node->name);
    else
      printf("find(%s)=false", *node->name);
  }
  printf("3.remove\n");
  for (size_t i = 1; i < argc; i++)
  {
    nm.remove(argv[i]);
  }
  printf("4.find\n");
  for (size_t i = 1; i < argc; i++)
  {
    if(nm.find(argv[i], &node)) 
      printf("find(%s)=true", *node->name);
    else
      printf("find(%s)=false", *node->name);
  }

  

  return 0;
}

#endif

#if 0

#include <stdio.h>
#include <malloc.h>

#define MAX 0xFFFFFFFF

void mmemset(void* p, char c, size_t s) {
  char* pp = (char*)p;
  while(s-->0)  *pp++ = c;
}

void upheap(int array[], int k) {
  int v;
  v = array[k];
//  array[0] = MAX;
  while(array[k/2]<=v && k > 0) {
    array[k] = array[k/2];
    k/=2;
  }
  array[k] = v;
}

void insert(int array[], int *hn, int v) {
  array[++(*hn)] = v;
  upheap(array, *hn);
}

void downheap(int array[], int n, int k) {
  int i;
  int v = array[k];
  while(k<=n/2) {
    i = k<<1;
    if(i<n&&array[i]<array[i+1]) i++;
    if(v>=array[i]) break;
    array[k] = array[i];
    k = i; 
  }
  array[k] = v;
}

int mdelete(int array[], int *n) {
  int v = array[1];
  array[1] = array[(*n)--];
  downheap(array, *n, 1);
  return v;
}

void heap_sort(int array[], int n) {
  int hn = 0;
  int i = 0;
  for(i = 1;i<=n;i++) {
    insert(array, &hn, array[i]);
  }
  for(i=hn;i>=1;i--) {
    array[i] = mdelete(array, &hn);
  }
  for (int j = 0; j < n; j++)
  {
    array[j] = array[j+1];
  }
}

void print_array(int array[], int size) {
  for (int i = 0; i < size; i++)
  {
    printf("[%4d]", array[i]);
  }
  printf("\n");
}

int main(int argc, char* argv[]) {
  int n = 0;
  int* arr;
  
  sscanf(argv[1], "%d", &n);
  printf("n=%d\n", n);
  arr = (int*)malloc(sizeof(int)*n);
  mmemset(arr, 0, sizeof(int)*n);
  for (size_t i = 0; i < n; i++)
  {
    sscanf(argv[i+2], "%d", &(arr[i]));
    printf(">>argv[%d]=%s, %d\n", i, argv[i+2], arr[i]);
  }

  printf("(1) : "); print_array(arr, n);
  heap_sort(arr, n);
  printf("(2) : "); print_array(arr, n);

  return 0;
}


#endif
#if 0
#include <iostream>
#include <vector>
#include <memory>
#include <cstdio>
#include <fstream>
#include <cassert>
#include <functional>
 
// helper class for runtime polymorphism demo below
struct B {
  virtual void bar() { std::cout << "B::bar\n"; }
  virtual ~B() = default;
};
struct D : B
{
    D() { std::cout << "D::D\n";  }
    ~D() { std::cout << "D::~D\n";  }
    void bar() override { std::cout << "D::bar\n";  }
};
 
// a function consuming a unique_ptr can take it by value or by rvalue reference
std::unique_ptr<D> pass_through(std::unique_ptr<D> p)
{
    p->bar();
    return p;
}
 
// helper function for the custom deleter demo below
void close_file(std::FILE* fp) { std::fclose(fp); }
 
// unique_ptr-based linked list demo
struct List {
  struct Node {
    int data;
    std::unique_ptr<Node> next;
    Node(int data) : data{data}, next{nullptr} {}
  };
  List() : head{nullptr} {};
  // N.B. iterative destructor to avoid stack overflow on long lists
  ~List() { clean(); }
  // copy/move and other APIs skipped for simplicity
  void push(int data) {
    auto temp{std::make_unique<Node>(data)};
    if(head) temp->next = std::move(head);
    head = std::move(temp);
  }

  void pop() {
      if(head == nullptr) return;
      head = std::move(head->next);
  }

  void remove(int data) {
      List::Node* node = head.get();
      while(node) {
          if(node->data == data) {
              List::Node* temp = node;
              node = node->next.get();
          }
          node = node->next.get();
      }
  }

  int find(int data) {
      Node* node = head.get();
      while(node) {
          if(node->data == data) {
              return 1;
          }
          if(node->next)
            node = node->next.get();
          else break;
      }
      return 0;
  }

  void clean() {
      while(head) { head = std::move(head->next); }
  }

  friend std::ostream& operator<<(std::ostream& os, const List&o);
private:
  std::unique_ptr<Node> head;
};

std::ostream& operator<<(std::ostream& os, const List& o) {
    List::Node* head = o.head.get();
    while(head) { os << head->data << ' '; head = head->next.get(); }
    return os;
}
 
int main()
{
  std::cout << "1) Unique ownership semantics demo\n";
  {
      auto p = std::make_unique<D>(); // p is a unique_ptr that owns a D
      auto q = pass_through(std::move(p)); 
      assert(!p); // now p owns nothing and holds a null pointer
      q->bar();   // and q owns the D object
  } // ~D called here
 
  std::cout << "2) Runtime polymorphism demo\n";
  {
    std::unique_ptr<B> p = std::make_unique<D>(); // p is a unique_ptr that owns a D
                                                  // as a pointer to base
    p->bar(); // virtual dispatch
 
    std::vector<std::unique_ptr<B>> v;  // unique_ptr can be stored in a container
    v.push_back(std::make_unique<D>());
    v.push_back(std::move(p));
    v.emplace_back(new D);
    for(auto& p: v) p->bar(); // virtual dispatch
  } // ~D called 3 times
 
  std::cout << "3) Custom deleter demo\n";
  std::ofstream("demo.txt") << 'x'; // prepare the file to read
  {
      std::unique_ptr<std::FILE, decltype(&close_file)> fp(std::fopen("demo.txt", "r"),
                                                           &close_file);
      if(fp) // fopen could have failed; in which case fp holds a null pointer
        std::cout << (char)std::fgetc(fp.get()) << '\n';
  } // fclose() called here, but only if FILE* is not a null pointer
    // (that is, if fopen succeeded)
 
  std::cout << "4) Custom lambda-expression deleter demo\n";
  {
    std::unique_ptr<D, std::function<void(D*)>> p(new D, [](D* ptr)
        {
            std::cout << "destroying from a custom deleter...\n";
            delete ptr;
        });  // p owns D
    p->bar();
  } // the lambda above is called and D is destroyed
 
  std::cout << "5) Array form of unique_ptr demo\n";
  {
      std::unique_ptr<D[]> p(new D[3]);
  } // calls ~D 3 times
 
  std::cout << "6) Linked list demo\n";
  {
    List l;
    for(long n = 1; n != 1'000'000; ++n) l.push(n);

    std::cout << "l.find(" << 999999 << ")=" << (l.find(999999) ? "true" : "false") << std::endl;
    l.pop();
    std::cout << "l.find(" << 999999 << ")=" << (l.find(999999) ? "true" : "false") << std::endl;

    std::cout << "destroying 1'000'000 nodes... ";
  } // destroys all 1 million nodes
  std::cout << "Done.\n";
}

#endif