#include <stdio.h>

template<typename T>
class MyNode {
	public:
	explicit MyNode(T data) : data{data} {}
	const T& getData() const { return data; }
	void setData(const T& data) { this->data = data; }
	MyNode* getNext() const { return next; }
	void setNext(MyNode* next) { this->next = next; }

	private:
	T data;
	MyNode* next;
};

template<typename T>
struct MyNodeViewerDef {
	void operator()(FILE* fp, const size_t& idx, const MyNode<T>& entry) const {
		fprintf(fp, "[%zu]=%s%s", entry->getData(), (entry->getNext() ? ", " : ""));
	};	
};

template<typename T, typename Viewer = MyNodeViewerDef<T>>
class MyList {
	public:
		MyList() : len{0}, head{nullptr}, tail{nullptr} {}
		void add(const T& data) {
			MyNode<T>* node = new MyNode<T>(data);

			MyNode<T>* prev = nullptr;
			MyNode<T>* entry = head;

			prev = tail;

#if 0
			while(entry) {
				prev = entry;
				entry = entry->getNext();
			}
#endif

			if(prev) {
				prev->setNext(node);
			}
			else {
				head = entry; 
			}
			tail = node;
			len++;
		}

		bool find(const T& target, T& node) {
			MyNode<T>* entry = head;
			while(entry) {
				if(entry->getData() == target) {
					node = entry->getData;
					return true;
				}
				entry = entry->getNext();
			}
			return false;
		}

		bool remove(const T& target, T& data) {
			MyNode<T>* prev = nullptr;
			MyNode<T>* entry = head;
			while(entry) {
				if(entry->getData() == target) {
					break;
				}
				prev = entry;
				entry = entry->getNext();
			}

			if(prev) {
				if(entry) {
					MyNode<T>* temp = prev->getNext();
					prev->setNext(entry->getNext());
					data = entry->getData();

					delete temp;

					len--;
					return true;
				}
			}
			else {
				if(entry) {
					// found with head
					MyNode<T>* temp = head;

					data = entry->getData();
					head = head->getNext();	

					delete temp;
					len--;
					return true;
				}
			}
			return false;
		}
	private:
		size_t len;
		MyNode<T> *head;
		MyNode<T> *tail;
		Viewer viewer;
};
