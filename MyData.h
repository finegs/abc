#pragma once
#include <string>

class MyData {
	private:
		int no;
		std::string name;
		std::string desc;
	public:
		const int getNo();
		const std::string getName();
		const std::string getDesc();
}
