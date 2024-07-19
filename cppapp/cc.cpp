#include <iostream>
#include <vector>
#include <string>
#include <set>
#include <unordered_set>
#include <vector>
#include <algorithm>
#include <memory>


class Item {
	protected:
		std::string name;
		std::string job;
		int         age;

	public:
		Item(std::string name, std::string job, int age)
			: name{name}, job{job}, age{age} {}

		virtual void work() const {
			std::cout << name << " is doing : " << job << std::endl;
		}

		virtual bool operator<(const Item& o) const {

			return age < o.age;
		}

		bool operator==(const Item& o) const {
			return name == o.name;
		}

		friend std::ostream& operator<<(std::ostream& os, const Item& o) {
			os << o.name << ", " << o.age << ", " << o.job;
			return os;
		}

		const std::string& getName() {
			return name; 
		}

		struct ItemHasher {
			size_t operator()(const Item& o) const {
				return std::hash<std::string>{}(o.name);
			}
		};
};

class SubItem : public Item {
	private:
		std::string subJob;

	public:
		SubItem(std::string name, std::string job, int age, std::string subJob)
				: Item{name, job, age} , subJob{subJob} {}
		virtual void work() const {
			std::cout << name << " is doing subJob : " << subJob << std::endl; 
		}
};

class AnotherSubJob : public Item {
	private:
		std::string anotherSubJob;

	public:
		AnotherSubJob(std::string name, std::string job, int age, std::string anotherSubJob)
			: Item{name, job, age} , anotherSubJob{anotherSubJob} {}

		virtual void work() const {
			std::cout << name << " is doing another subjob : " << anotherSubJob << std::endl;
		}
};

int main(int argc, char* argv[]) {

	//std::unordered_set<Item, Item::ItemHasher> s;	
	std::set<std::shared_ptr<Item>> s;	
	s.insert(std::make_shared<Item>("Item", "Basement", 100));
	s.insert(std::make_shared<SubItem>("SubItem", "Student", 51, "Part Time"));
	s.insert(std::make_shared<SubItem>("SubItem", "Teacher", 35, "Reguler"));
	s.insert(std::make_shared<AnotherSubJob>("AnotherSubJob", "Developer", 45, "System Admin"));

//	std::for_each(s.begin(), s.end(), [](Item* o) { o->work(); });
	std::for_each(s.begin(), s.end(), [](auto& o) { o.get()->work(); });

	return 0;
}


#if 0
template<typename Transport>
class service : private Transport  // private inheritance from the Transport policy
{
public:
    void transmit() {
        this->send(...);  // send using whatever transport was supplied
    }
};
// TCP transport policy
class tcp {
public:
    void send(...);
};
// UDP transport policy
class udp {
public:
    void send(...);
};
 
service<tcp> service(host, port); 
service.transmit(...); // send over TCP

#endif

