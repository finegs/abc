#include <iostream>
#include <concepts>

template<typename T>
concept Number = std::integral<T> || std::floating_point<T>;

template <typename T>
requires Number<T>
class WrapperNumber {
	public:
		WrapperNumber(T num) : m_num(num){}
		T getNum() { return m_num; }
	private:
		T m_num;
};

int main(){
	WrapperNumber wn{42};

	std::cout << wn.getNum() << std::endl;

	return 0;
}
