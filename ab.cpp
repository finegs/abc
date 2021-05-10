#include <iostream>
#include <cassert>
#include <string>
#include <memory>

#define massert(expression, message) assert((expression) && message)

class PDesc {
	public:
		PDesc(const char* name, const char* desc) {
			massert(name != NULL , "Name must provied");
			name_ = std::make_unique<std::string>(name);

			massert(desc != NULL, "Name must provied");
			desc_ = std::make_unique<std::string>(desc);
		}

		const char* getName() const { return name_.get()->c_str(); }
		const char* getDesc() const { return desc_.get()->c_str(); }

	private:
		std::unique_ptr<std::string> name_;
		std::unique_ptr<std::string> desc_;
};

int main() {
	PDesc pd(NULL, "a");
	return 0;
};
