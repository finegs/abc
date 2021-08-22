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