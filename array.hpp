#include <iostream>

template <typename T, unsigned int N>
class Array {
	T data[N];

	public:
	Array(T (&arr)[N]) {
		for(int i = 0;i<N;++i) {
			data[i] = arr[i];
		}
	}

	T* getArray() { return data; }

	unsigned int size() { return N; }

	void print() {
		for(int i = 0;i < N;++i) {
			std::cout << data[i] << ", ";
		}
		std::cout << std::endl;
	}
};


