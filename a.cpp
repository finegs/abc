
#if 1

#include <iostream>
#include <cstdint>
#include <complex>
#include <atomic>
#include <thread>
#include <queue>
#include <deque>
#include <utility>
#include <mutex>
#include <condition_variable>
#include <chrono>

using namespace std;

struct Item;

class AA {
	public:
		std::atomic_bool run;
		std::mutex mtx;
		std::condition_variable cond_var;
		std::deque<std::pair<std::string, Item>> que;
		int a;
		int b;
		
		void push_work(Item itm);
		Item pop_work();
		void do_read();
		void do_work();
		friend std::ostream& operator<<(std::ostream& os, const AA& o);	
		friend std::istream& operator>>(std::istream& os, AA& o);	
};

struct Item {
	std::string name;
	std::string desc;
	uint32_t value;

	friend std::istream& operator>>(std::istream& is, Item& o);
	friend std::ostream& operator<<(std::ostream& os, const Item& o);
};

std::ostream& operator<<(std::ostream& os, const AA& o) {
	os << "a : " << o.a << ", b : " << o.b << std::endl;
	return os;
}

std::istream& operator>>(std::istream& is, Item& o) {
	is >> o.name >> o.desc >> o.value;
	return is;
}

std::ostream& operator<<(std::ostream& os, const Item& o) {
	os << "{\"name\":" << o.name << ", " << "\"desc\":" << o.desc << ", \"value\":" << o.value << "}";
	return os;
}

void AA::push_work(Item itm) {
	std::lock_guard<std::mutex> lg(mtx);
	que.push_back({itm.name, itm});

	cond_var.notify_all();
}

void AA::do_read() {
	std::string input;
	while(run) {
		std::cout << "Enter : "; std::cout.flush();
		std::getline(std::cin, input); std::cin.clear();

		if("exit" == input || "quit" == input) {
			run= false;
		}
		else {

			std::string str;
			std::stringstream ss(input);

			Item itm;
			ss >> itm;
			push_work(itm);

			std::cout << "Inputs : " << itm << "\n";
		}
	}
}

Item AA::pop_work() {
		std::unique_lock<std::mutex> lk;
		cond_var.wait(lk, [&] { return !que.empty();});

		std::pair<std::string, Item> work  = que.front();
		que.pop_front();

		lk.unlock();

		return work.second;
}

void AA::do_work() {
	while(run) {
		Item item = pop_work();

		std::cout << "do_work : " << item << std::endl;
	}
}

int main() {
	AA aa{.run{true}, .que{}, .a=100, .b=100};

	std::thread worker(&AA::do_work, std::ref(aa));
	std::thread reader(&AA::do_read, std::ref(aa));

	while(aa.run) {
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}

	reader.join();	
	worker.join();	

#if 0
	int rc = 0;
	uint8_t a = 5, b = 9, c= 255;
	AA aa{.a=100};
	rc = printf("a=%u, b=%u, c=%u\n", a,b,c);
	printf("rc : %d\n", rc);

	cout << aa;

	std::string ab{"aa"};
#endif

#if 0
	double complex z1 = 1.0 + 3.0*I;
	printf("z1 = %.02f + %.02fi\n", crealf(z1), cimagf(z1));
#endif
//	double imaginary z = 3*I;
//	z = 1/z;
//	printf("1/(3.0i) = %+.lfi\n", cimag(z));
	return 0;
}

#endif
