
#if 1

#include <iostream>
#include <vector>
//#include <string>
#include <algorithm>
#include <unordered_map>

struct Item {
	int x,y;
	friend std::ostream;

	bool operator==(const Item& o) const {
		return x == o.x && y == o.y;
	}
};

template<> struct std::less<Item>
{
	bool operator()(const Item& a, const Item& b) const 
	{
		return a.x < b.x || a.y < b.y;
	}
};
template<> struct std::hash<Item> 
{
	size_t operator()(const Item& o) const
	{
		return (o.x<<5)&o.y;
	}
};

std::ostream& operator<<(std::ostream& os, const Item& o) {
	os << "{" << "\"x:\""<< o.x << ", " << "\"y\":" << o.y << "}";
	return os;
}

using namespace std;
int main(int argc, char* argv[]) {

	vector<std::string> v = {"hi", "hello", "abc"};

	size_t l = 0;
	cout << "for_each : [";
	std::for_each(v.cbegin(), v.cend(), [&](auto& s) { cout << (++l>0?",":"") << s << " "; });
	cout << "]" << endl;

	cout << "for : [";
	for(const auto& s : v) {
		cout << (l++>0 ? ", " : "") << s;
	}
	cout << "]" << std::endl;;

	vector<Item> vv{{1,2}, {3,4},{5,6}};
	l = 0;
	cout << "vv : ";
	std::for_each(vv.cbegin(), vv.cend(), [&](auto& s) { cout << (l>0?"," :"") << "{" << "x:"<< s.x << ", " << "y:"<< s.y << "}";});
	cout << "}" << std::endl;

	Item ii{1,1};
	auto rr = std::find_if_not(vv.cbegin(), vv.cend(), [&](auto& i) { return ii == i; });
	if(rr != vv.cend()) {
		cout << "Found " << ii << std::endl;
	}
	else {
		cout << "Not found " << ii << std::endl;
	}

	unordered_map<const char*,Item> um; 
	um.insert(std::make_pair<const char*, Item>("1_1", {1,1}));
	um.insert(std::make_pair<const char*, Item>("1_2", {1,2}));
	um.insert({{"1_3"}, {1,3}});

	auto it = um.find("1_1");
	if(it != um.cend()) 
		cout << "um.find(\"1_1\") : OK" <<  ", Result : {" << it->first << ", " << it->second << "}";
	else
		cout << "um.find(\"1_1\") : NG";
	cout << std::endl;

	um.erase("1_1");

	it = um.find("1_1");
	if(it != um.cend()) 
		cout << "um.find(\"1_1\") : OK" <<  ", Result : {" << it->first << ", " << it->second << "}";
	else
		cout << "um.find(\"1_1\") : NG";
	cout << std::endl;


	return EXIT_FAILURE;
}


#endif


#if 0
#include <stdio.h>
#include <malloc.h>

void mmemset(void*p,char c, size_t l) {
	char* cc = (char*)p;
	while(l-->0) cc[l]=c;
}

size_t mstrlen(const char str[]) {
  size_t l = 0;
  while(str[l] != '\0')l++;
  return l;
}

int mstrcmp(const char s1[], const char s2[]) {
  if(!s1 || !s2) return 0;
  size_t i = 0;
  while(s1[i] != '\0' && s1[i] == s2[i]) ++i;
  return s1[i] - s2[i];
}

void mstrcpy(char dst[], const char src[]) {
  int i  = 0;
  while((dst[i] = src[i]) != '\0') ++i;
}

size_t mstrhash(const char str[], const size_t mode) {
  size_t h = 8591;
  char c = 0;
  while((c=*str++) != '\0') {
    h = ((h<<5)+h+c)%mode;
  }
  return h % mode;
}

struct Node {
  Node(const char name[], const char desc[], int point) 
    : point{point} {
      mstrcpy(this->name, name);
      mstrcpy(this->desc, desc);
  }

  void setDesc(const char desc[]) { mstrcpy(this->desc, desc); }
  void setPoint(int point) { this->point = point; }
  void setNext(Node* next) { this->next = next; }

  char name[64];
  char desc[1024];
  int point;
  Node* next;
};

template<size_t MAP_CAPACITY = 10000>
struct NodeMap {
  NodeMap(size_t _mapCapacity = MAP_CAPACITY) 
          : mapCapacity{_mapCapacity}, mapSize{0} 
  {
    nodeList = (Node**)malloc(sizeof(Node*) * mapCapacity);
    for (size_t i = 0; i < mapCapacity; ++i)
    {
      // nodeList[i] = (Node*)malloc(sizeof(Node)); 
      nodeList[i] = nullptr;
    }
  }

  ~NodeMap() {
    for (size_t i = 0; i < mapCapacity; i++)
    {
      if(nodeList[i]) free(nodeList[i]);
    }
    free(nodeList);
  }

  void put(const char name[32], const char desc[128]) {
    size_t h = mstrhash(name, mapCapacity);
    Node* prev = nullptr;
    Node* node = nodeList[h];
    while(node) {
      if(mstrcmp(name, node->name) == 0) break;
      prev = node;
      node = node->next;
    }

    if(prev) {
      if(node) {
        mstrcpy(node->desc, desc);
      }
      else {
        node = (Node*)malloc(sizeof(Node));
        mstrcpy(node->name, name);
        mstrcpy(node->desc, desc);

        prev->next = node;

        mapSize++;
      }
    }
    else {
      node = (Node*)malloc(sizeof(Node));
      mstrcpy(node->name, name);
      mstrcpy(node->desc, desc);

      nodeList[h] = node;

      mapSize++;
    }
  }

  Node** nodeList;
  size_t mapCapacity;
  size_t mapSize;
};


int main(int argc, char* argv[]) {

  NodeMap<30000> nm;

  printf("#1 mstrcmp(%s,%s)=%s\n", argv[0],argv[0],mstrcmp(argv[0],argv[0])==0? "OK":"NG");
  char bb[100];
  mmemset(bb, '\0', 100);
  mstrcpy(bb, argv[0]);
  printf("#2 mstrcmp(%s,%s)=%s\n", argv[0],bb, mstrcmp(argv[0],bb)==0? "OK":"NG");

  for (int i = 1; i < argc; ++i) {
	  printf("mstrlen(%s)=%d\n", argv[i], mstrlen(argv[i]));
	  if(i%2==1) 
		nm.put(argv[i], argv[i+1]);	
  }

  return 0;
}

#endif
#if 0
=======
  if(!src) return; 
  size_t i = 0;
  while((dst[i] = src[i]) != '\0') ++i;
}

size_t mstrhash(const char s[], size_t mod) {
  size_t h = 8391;
  char c =0;
  while((c = *s++) != '\0') {
    h  =((h<<5)+h+c) % mod; 
  }
  return h%mod;
}

class AAA {
  public:
    AAA(const char str[] = NULL) {
      size_t len = mstrlen(str);
      AAA::str = (char*)malloc(len+1);
      mstrcpy(AAA::str, str);
      AAA::str[len] = '\0';
    }
    ~AAA(){
      printf("~AAA\n");
      if(str) free(str);
    };

    void setStr(const char* str)  {
      if(AAA::str) free(AAA::str);
      size_t len = mstrlen(str);
      AAA::str = (char*)malloc(len+1);
      mstrcpy(AAA::str, str);
      AAA::str[len] = '\0';
    }

    private:
      char *str;
};

class AA
{
public:
  AA(AAA* arr) 
    : arr{arr} {}
  ~AA() = default;

private:
  AAA* arr;
};

int main(int argc, char* argv[]) {
  AAA arr[argc-1];
  for (size_t i = 1; i < argc; i++)
  {
    arr[i-1].setStr(argv[i]);
  }
  
  AA aa{arr};
}

#endif
#if 0

#include <stdio.h>
#include <malloc.h>

template<typename K, typename V>
class HashNode {
  public:
    HashNode(const K& key, const V& value) 
      : key(key), value(value), next(nullptr) {}

    K getKey() const { return key; }
    V getValue() const { return value; }
    void setValue(V value) { HashNode::value = value; }
    HashNode* getNext() const { return next; }
    void setNext(HashNode* next) { HashNode::next = next; }
  private:
    K key;
    V value; 
    HashNode* next;
};

template <typename K, unsignd long M>>
struct HashKeyHash {
  unsigned long operator()(const K& key) const {
    return reinterpret_pointer_cast<unsigned long>(key % M);
  }
};

// Hash map class template
template <typename K, typename V, typename F = KeyHash<K>>
class HashMap {
public:
    HashMap() {
        // construct zero initialized hash table of size
        table = new HashNode<K, V> *[TABLE_SIZE]();
    }

    ~HashMap() {
        // destroy all buckets one by one
        for (int i = 0; i < TABLE_SIZE; ++i) {
            HashNode<K, V> *entry = table[i];
            while (entry != NULL) {
                HashNode<K, V> *prev = entry;
                entry = entry->getNext();
                delete prev;
            }
            table[i] = NULL;
        }
        // destroy the hash table
        delete [] table;
    }

    bool get(const K &key, V &value) {
        unsigned long hashValue = hashFunc(key);
        HashNode<K, V> *entry = table[hashValue];

        while (entry != NULL) {
            if (entry->getKey() == key) {
                value = entry->getValue();
                return true;
            }
            entry = entry->getNext();
        }
        return false;
    }

    void put(const K &key, const V &value) {
        unsigned long hashValue = hashFunc(key);
        HashNode<K, V> *prev = NULL;
        HashNode<K, V> *entry = table[hashValue];

        while (entry != NULL && entry->getKey() != key) {
            prev = entry;
            entry = entry->getNext();
        }

        if (entry == NULL) {
            entry = new HashNode<K, V>(key, value);
            if (prev == NULL) {
                // insert as first bucket
                table[hashValue] = entry;
            } else {
                prev->setNext(entry);
            }
        } else {
            // just update the value
            entry->setValue(value);
        }
    }

    void remove(const K &key) {
        unsigned long hashValue = hashFunc(key);
        HashNode<K, V> *prev = NULL;
        HashNode<K, V> *entry = table[hashValue];

        while (entry != NULL && entry->getKey() != key) {
            prev = entry;
            entry = entry->getNext();
        }

        if (entry == NULL) {
            // key not found
            return;
        }
        else {
            if (prev == NULL) {
                // remove first bucket of the list
                table[hashValue] = entry->getNext();
            } else {
                prev->setNext(entry->getNext());
            }
            delete entry;
        }
    }

private:
    // hash table
    HashNode<K, V> **table;
    F hashFunc;
};

struct mstringkeyhash {
  unsigned long operator()(MString& o) const {
    return mstrhash(o.str);
  }
};

int mstrcmp(const char s1[], const char s2[]) {
  size_t i = o;
  while(s1[i] != '\0' && s1[i] == s2[i]) ++i;
  return s1[i] - s2[i];
}

size_t mstrlen(const char s[]) {
  size_t l = 0;
  while(s[l] != '\0') ++l;
  return l;
}

size_t mstrcpy(char dst[], const char src[]) {
  size_t i = 0;
  while((dst[i] = src[i]) != '\0') ++i;
}

size_t mstrhash(const char s[], size_t mod) {
  size_t h = 8391;
  char c =0;
  while((c = *s++ != '\0') {
    h  =((h<<5)+h+c) % mod; 
  }
  return h%mod;
}
struct mstring {
  mstring(const char str[]) 
    : len(mstrlen(str+1), capacity{8096}) {
    str = new char[len+1];
    mstrcpy(mstring::str, str);
    mstring::str[len] = '\0';
  }
  ~mstring() = default;

  bool operator==(const mstring& o) const {
    return 0 == mstrcmp(mstring::str, o.str);
  }

  int compare(const mstring& o) const {
    return mstrcmp(mstring::str, o.str);
  }
  char* str;
  size_t len;
  size_t capacity;
}


int main(int argc,char* argv[]) {

  HashMap<mstring, mnode, mstringkeyhash> hmap;



  return 0;
}


#endif


#if 0
#include <stdio.h>
#include <malloc.h>
#include <memory>

void mmemset(void* p, char c, size_t l) {
  char* cp = (char*)p;
  while(l--) *cp++ = c;
}

size_t mstrlen(const char str[]) {
  size_t l = 0;
  while(str[l] != '\0') ++l;
  return l;
}
size_t mstrhash(const char str[], size_t mod) {
  size_t h = 5381;
  char c = 0;
  while((c=*str++) != '\0') {
    h = ((h<<5)+h+c)%mod;
  }
  return h % mod;
}
void mstrcpy(char dst[], const char src[]) {
  int i =0;
  while((dst[i] = src[i]) != '\0') ++i;
}

int mstrcmp(const char str1[], const char str2[]) {
  int i = 0;
  while(str1[i] != '\0' && str1[i] == str2[i]) ++i;
  return str1[i]-str2[i];
}

struct Node {
  void* key;
  void* data;
  Node* next;

  ~Node() {
    Node* n = next;
    while(n) {
      Node* p = n;
      n = n->next;
      delete(p);
    }
  }
};

#define DEFAULT_HASH_MAP_MAX_SIZE 100000
#define DEFAULT_STRING_SIZE 256
struct HashMap {
  HashMap(
   size_t (*key_hash)(void* data, size_t mod),
   int (*key_match)(void* key1, void* key2),
   void (*destroy)(void* data),
   void (*handle)(void* data),
   size_t max_size = DEFAULT_HASH_MAP_MAX_SIZE)
    : key_hash{key_hash}, 
      key_match{key_match}, 
      destroy{destroy}, 
      handle(handle),
      size{0}, 
      max_size{max_size}
  {
      nodeList = (Node**)malloc(max_size*sizeof(Node*));
      mmemset(nodeList, 0, sizeof(Node*)*max_size);
  }

  ~HashMap() {
    for (size_t i = 0; i < max_size; ++i)
    {
      Node* p = nodeList[i];
      while(p) {
        destroy(p->data);

        Node* prev = p;
        p = p->next;
        delete(prev);
      }
    }
    free(nodeList);
    nodeList = NULL;
  }

  int remove(void* key, void** data) {
    size_t h = key_hash(key, max_size);
    Node* prev = NULL;
    Node* node = nodeList[h];
    while(node && !key_match(key, node->key)) {
      prev = node;
      node = node->next;
    } 

    if(!node) return -1;

    // Found
    *data = node->data;
    if(prev) { // Found in List
      prev->next  = node->next;
    }
    else { // Found in head
      nodeList[h] = node->next;
    }
    node->data = NULL;
    free(node); 
    size--;
    return 0;
  }

  void insert(void* key, void* data) {
    size_t h = key_hash(key, max_size);
    Node* prev = NULL;
    Node* node = nodeList[h];
    while(node && !key_match(key, node->key)) {
      prev = node;
      node = node->next;
    }

    if(node) { // update
      node->key = key;
      handle(node->data);
    }
    else { // insert
      node = (Node*)malloc(sizeof(Node));
      mmemset(node, 0, sizeof(Node));
      node->key = key;
      node->data = data;
      handle(node->data);

      if(prev) {
        node->next = prev->next;
        prev->next = node; 
      }
      else {
        nodeList[h] = node;
      }
      size++;
    }
  }

  void* get(void* key) const {
    size_t h = key_hash(key, max_size);
    Node* prev = NULL;
    Node* node = nodeList[h];
    while(node && !key_match(key, node->key)) {
      prev = node;
      node = node->next;
    }
    if(node) return node->data;
    return NULL;
  }

  size_t (*key_hash)(void* data, size_t mod);
  int (*key_match)(void* key1, void*key2);
  void (*destroy)(void* data);
  void (*handle)(void* data);
  Node** nodeList;
  size_t size;
  size_t max_size;
};

size_t string_hash(void* key, size_t mod) {
  return mstrhash((const char*)key, mod);
}

int string_match(void* key1, void* key2) {
  return mstrcmp((const char*)key1, (const char*)key2) == 0;
}

struct mynode {
  char* name; 
  int point;

  mynode(const char name[], int point = 0)
    : point{point} {
      size_t len = mstrlen(name);
      this->name = (char*)malloc(len+1);
      mstrcpy(this->name, name);
      this->name[len] = '\0';
  };

  ~mynode() {
    delete(name);
  }
};


void mynode_destroy(void* data) {
  mynode* node = (mynode*)data;
  delete(node);
}

void mynode_handle(void* data) {
  mynode* node = (mynode*)data;
  node->point+=1;
}

struct mstring;

#if 0
struct BB {
  BB() {
    printf("BB()\n");
  }
  ~BB() {
    printf("~BB()\n");
  }
};
struct AAA {
  BB bb;
};
#endif

int main(int argc, char* argv[]) {

 #if 0 
  printf("#1.---------------------\n");
  HashMap m{string_hash, string_match, mynode_destroy, mynode_handle};
  for (size_t i = 1; i < argc; ++i)
  {
    mynode* node = new mynode{argv[i], 0};
    m.insert(argv[i], node);
  }

  printf("#2.---------------------\n");
  mynode* node = NULL;
  for (size_t i = 1; i < argc; ++i)
  {
    char* name = argv[i];
    node = (mynode*)m.get(name);
    if(node)
      printf("\t[%5d] : map[%10s]={%10s,%5d}\n", i, node->name, node->name, node->point) ;
    else
      printf("\t[%5d] : map[%10s]=\n", i, name) ;
  }
 #endif 

#if 0
  {
    printf("#1.--------\n");
    AAA a;  
    printf("#2.--------\n");
    printf("#3.--------\n");
  }
#endif



  return 0;
}

struct mstring {
  mstring(const char str[]) 
    :len{DEFAULT_STRING_SIZE}
    {
      this->str = (char*)malloc(len+1);
      if(!this->str) { fprintf(stderr, "fail to malloc %s:%d %s\n", __FILE__, __LINE__, __FUNCTION__);len = 0; return; }
      mstrcpy(this->str, str);
      this->str[len] = '\0';
    }

    mstring(const mstring& o) {
      docopy(o);
    }

    mstring& operator=(const mstring& o) {
      if(this==&o) return *this;
      docopy(o);
      return *this;
    }

    mstring(mstring&& o) {
      if(str) free(str);
      domove(std::forward<mstring>(o));
    }

    mstring& operator=(mstring&& o) {
      if(this==&o) return *this;
      domove(std::forward<mstring>(o));
    }

    void docopy(const mstring& o) {
      if(this->str) free(this->str);
      this->len = o.len;
      this->str = (char*)malloc(len+1);
      if(!this->str) { fprintf(stderr, "fail to malloc %s:%d %s\n", __FILE__, __LINE__, __FUNCTION__);len = 0; return; }
      mstrcpy(this->str, o.str);
      this->str[len] = '\0';
   }

   void domove(mstring&& o) {
     if(this==&o) return;
     if(str) free(str);
     str = std::move(o.str);
     len = o.len;
   }

    ~mstring() {
      if(!str) return;
      free(str);
      str = nullptr;
      len = 0;
    }
  char* str;
  size_t len;

};


#endif


#if 0
#include <stdio.h>
#include <malloc.h>
#include <time.h>

/* returns an array of arrays of char*, all of which NULL */
char ***alloc_matrix(unsigned rows, unsigned columns) {
    char ***matrix = (char***)malloc(rows * sizeof(char **));
    unsigned row = 0;
    unsigned column = 0;
    if (!matrix) abort();

    for (row = 0; row < rows; row++) {
        matrix[row] = (char**)calloc(columns, sizeof(char *));
        if (!matrix[row]) abort();
        for (column = 0; column < columns; column++) {
            matrix[row][column] = NULL;
        }
    }
    return matrix;
}

/* deallocates an array of arrays of char*, calling free() on each */
void free_matrix(char ***matrix, unsigned rows, unsigned columns) {
    unsigned row = 0;
    unsigned column = 0;
    for (row = 0; row < rows; row++) {
        for (column = 0; column < columns; column++) {
            /*    printf("column %d row %d\n", column, row);*/
            free(matrix[row][column]);
        }
        free(matrix[row]);
    }
    free(matrix);
}


int main(int argc, char **argv) {
    if(argc < 2) { printf("Usage : %s size1 size2\n", argv[0]); return 0; }
    printf("Start.\n");
     int i;
    srand(time(NULL));
    int randomnumber;
    int size = atoi(argv[1]);
    void *p[size];
    for (i = 0; i < size; i++) {
        randomnumber = rand() % 10;
        p[i] = malloc(1024 * 1024 * randomnumber);
    }

    for (i = size-1; i >= 0; i--) {
        free(p[i]);
    }

    int x = atoi(argv[2]);
    char *** matrix = alloc_matrix(x, x);
    free_matrix(matrix, x, x);
    printf("End.\n");
    return (0);
}

#endif

#if 0
#include <stdio.h>
#include <malloc.h>

static void mmemset(void* ptr, char c, size_t len) {
  char* cp = (char*)ptr;
  while(len--) {
    *cp++ = c;
  }
}
static void mstrcpy(char a[], char b[]) {
  int i = 0;
  while((a[i] = b[i]) != '\0') {++i;}
}

static int mstrcmp(const char a[], const char b[]) {
  int i = 0;
  while((a[i] != '\0') && a[i] == b[i]) ++i;
  return a[i] - b[i];
}

static const size_t NAME_LEN = 16;

struct Node {
  char name[16];
  Node* next;
};

struct NodeMap {
  size_t len;
  Node** nodeList;
  NodeMap(size_t len = 100000) :len(len) {
    nodeList = (Node**)malloc(sizeof(Node*)*len);
    for(int i = 0;i < len;++i) {
     nodeList[i] = (Node*)malloc(sizeof(Node));
     mmemset(nodeList[i]->name, 0, NAME_LEN);
   }
  }

  ~NodeMap() {
    for(int i = 0;i < len;++i) {
     free(nodeList[i]);
   }
   free(nodeList);
   printf("~NodeMap()\n");
  }
};

NodeMap nodeMap;

int main(int argc, char* argv[]) {

  for(int i =1;i<argc;++i) {
    mstrcpy(nodeMap.nodeList[i-1]->name, argv[i]); 
  }

  for(int i = 1;i<argc;++i) {
    printf("nodeMap.nodeList[%d]=%s\n", i, nodeMap.nodeList[i-1]->name);
  }

  return 0;
}

#endif

#if 0


#include <stdio.h> #define parent(x) (x-1)/2 void heap(int *data, int num){ for(int i=1; i<num; i++){ int child = i; while(child > 0){ int root = parent(child); if(data[root] < data[child]){ int temp = data[root]; data[root] = data[child]; data[child] = temp; } child = root; } } } int main(void){ int num = 9; int data[] = {15, 4, 8, 11, 6, 3, 1, 6, 5}; heap(data, num); // 힙을 만든다. for(int i=num-1; i>=0; i--){ // 가장큰 숫자(root)를 맨 마지막 원소와 스왑 int temp = data[i]; data[i] = data[0]; data[0] = temp; // 맨마지막원소(가장큰원소)를 제외하고 다시 힙을 만든다. heap(data, i); } // 결과 출력 for(int i=0; i<num; i++){ printf("%d ", data[i]); } return 0; }








#endif
#if 0

#include <stdio.h>
 
void swap(int* a, int* b)
{
    int t = *a;
    *a = *b;
    *b = t;
}
 
void maxHeapify(int array[], int size, int index)
{
    int largest = index;    // Initialize the largest as root
    int left = (index << 1) + 1;   // left = 2*index + 1
    int right = (index + 1) << 1;    // right = 2*index + 2
 
    // See if left child of root exists and is greater than root
    if (left < size && array[left] > array[largest])
        largest = left;
 
    // See if right child of root exists and is greater than the largest so far
    f (right < size && array[right] > array[largest])
        largest = right;
 
    // Change root, if needed
    if (largest != index)
    {
        swap(&array[largest], &array[index]);
        maxHeapify(array, size, largest);
    }
}
 
void heapSort(int array[], int size)
{
    int i;
    // Create an initial max heap.
    // Start from bottommost and rightmost internal mode and
    // heapify all internal modes in bottom up way
    for (i = (size - 2) / 2; i >= 0; i--)
        maxHeapify(array, size, i);
 
    // Repeat reduce and heapify steps while heap size is greater than 1.
    while (size > 1)
    {
        // The largest item in Heap is stored at the end.
        swap(&array[0], &array[size - 1]);
        // Reduce heap size
        size--;
        // Heapify the root of tree
        maxHeapify(array, size, 0);
    }
}
 
void printArray(int arr[], int size)
{
    int i;
    for (i = 0; i < size; i++)
        printf("%d ", arr[i]);
    printf("\n");
}
 
int main(int argc, char** argv)
{
    int data[] = { 3,7,2,11,6,8,5,12,2,14,13,9,1,10,4 };
    int size = sizeof(data) / sizeof(int);
 
    heapSort(data, size);
    printf("Sorted data is\n");
    printArray(data, size);
 
    return 0;
}


#endif
#if 0

#include <stdio.h>
#include <malloc.h>

struct Node {
  void* data;
};

struct Heap {
  Heap(int (*data_comp)(void*,void*), 
    void (*data_swap)(void* a, void* b), 
    void (*data_destroy)(void**), 
    size_t max_size = 10000) 
      : data_comp(data_comp), 
        data_swap(data_swap), 
        data_destroy(data_destroy), 
        size(0), 
        max_size(max_size) {
      nodeList = (Node**)malloc(sizeof(int*)*max_size);
      for (size_t i = 0; i < max_size; i++)
      {
        nodeList = NULL;
      }
  }

  ~Heap() {
    for (size_t i = 0; i < max_size; i++)
    {
      if(nodeList[i]) data_destroy(nodeList[i]);
    }
    free(nodeList);
    nodeList = NULL;
  }

  void insert(void* data) {
    if(size>=max_size) {
      if(nodeList = (Node**)realloc(nodeList, max_size * 2)) {
          max_size *= 2;
          for (size_t i = size; i < max_size; i++) nodeList[i] = NULL;
      }
      else {
        fprintf(stderr, "Fail to realloc(nodeLis, %u\n", max_size*2);
        return;
      }
    }

    size_t cur = size;
    nodeList[cur] = (Node*)data;
    size_t p = (cur-1)/2;
    while(cur>0 && data_comp(nodeList[cur], nodeList[p])>0) {
      data_swap(nodeList[cur], nodeList[p]);
      p = (cur-1)/2;
    }

    size++;
  }

  void* remove() {
    Node** node;

    if(size() < 1) return NULL;

    node = nodeList[0];
    size_t cur = 0, child = cur*2;
    nodeList[cur] = nodeList[size--];
    while(child < size) {
      if(child + 1 < size && data_comp(nodeList[child], nodeList[child+1]) < 0) {
        child++;
      }
      data_swap(nodeList[cur], dataList[child]);
    }

    return node;
  }
  Node **nodeList;
  int (*data_comp)(void* a, void* b);
  void (*data_swap)(void* a, void* b);
  void (*data_destroy)(void** data);

  size_t getSize() { return size; }
  size_t getMaxSize() { return max_size; }

  size_t size;
  size_t max_size;
};

int main(int argc, char* argv[]) {

  return 0;
}


#endif 


#if 0

#include <stdio.h>
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
        hash_table->elements = (key_value_pair**)realloc(hash_table->elements, hash_table->max = hash_table->number_of_elements*2);
        for (int i = hash_table->number_of_elements; i < hash_table->max; i++) {
              hash_table->elements[i] = NULL;
        }

//        return 0; // FULL
    }

    for (try_num = 0; try_num < max_number_of_retries; try_num++) {

        hash = hash_fun(data->key, try_num, hash_table->max);

        if (NULL == hash_table->elements[hash]) { // an unallocated slot
          hash_table->elements[hash] = data;
          hash_table->number_of_elements++;
          return EXIT_SUCCESS;
        } else if (data->key == hash_table->elements[hash]->key) {
          hash_table->elements[hash] = data;
          return EXIT_SUCCESS;
        }
    }
    return EXIT_FAILURE;
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
    //      printf("alloc of slot failed\n");
    //      while (1);
    //  }
    //  pHashTable->elements[i] = pa_slot;
    //  pHashTable->elements[i]->key = 0;
    //}

    return EXIT_SUCCESS;
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
        return EXIT_FAILURE;
    }
    strcpy(pMyStruct->pValue, buffer);
    pMyStruct->key = a_key;

    return EXIT_SUCCESS;
}


// Assumes the hash table has already been allocated.
int add_key_val_pair_to_dict(struct hash_table *pHashTable, int key, const char pBuff[]) {

    int rc;
    struct key_value_pair *pKeyValuePair;

    if (NULL == pHashTable) {
        printf("Hash table is null.\n");
        return EXIT_FAILURE;
    }

    // Allocate the dictionary key value pair struct
    pKeyValuePair = (struct key_value_pair *) malloc(sizeof(struct key_value_pair));
    if (NULL == pKeyValuePair) {
        printf("Failed to allocate key value pair struct.\n");
        return EXIT_FAILURE;
    }


    rc = make_dict_entry(key, pBuff, pKeyValuePair);  // a_hash_table[1221] = "abba"
    if (EXIT_FAILURE == rc) {
        printf("Failed to add buff to key value pair struct.\n");
        return EXIT_FAILURE;
    }


    rc = hash_insert(pKeyValuePair, pHashTable);
    if (EXIT_FAILURE == rc) {
        printf("insert has failed!\n");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
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
    if (EXIT_FAILURE == rc) {
        printf("insert has failed!\n");
        return EXIT_FAILURE;
    }
    rc = add_key_val_pair_to_dict(&a_hash_table, 2211, "bbaa");
    if (EXIT_FAILURE == rc) {
        printf("insert has failed!\n");
        return EXIT_FAILURE;
    }
    rc = add_key_val_pair_to_dict(&a_hash_table, 1122, "aabb");
    if (EXIT_FAILURE == rc) {
        printf("insert has failed!\n");
        return EXIT_FAILURE;
    }
    rc = add_key_val_pair_to_dict(&a_hash_table, 2112, "baab");
    if (EXIT_FAILURE == rc) {
        printf("insert has failed!\n");
        return EXIT_FAILURE;
    }
    rc = add_key_val_pair_to_dict(&a_hash_table, 1212, "abab");
    if (EXIT_FAILURE == rc) {
        printf("insert has failed!\n");
        return EXIT_FAILURE;
    }
    rc = add_key_val_pair_to_dict(&a_hash_table, 2121, "baba");
    if (EXIT_FAILURE == rc) {
        printf("insert has failed!\n");
        return EXIT_FAILURE;
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

size_t mstrlen(const char str[]) {
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
