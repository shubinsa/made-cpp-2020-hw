#include <iostream>
using namespace std;

template<typename T>
class Allocator {
public:
	using value_type = T;
	using pointer = T*;
	using const_pointer = const T*;
	using reference = T&;
	using const_reference = const T&;
	using size_type = std::size_t;
	using difference_type = std::ptrdiff_t;
	using propagate_on_container_move_assignment = std::true_type;

	//ругается на шаблон - не могу понять
	//using rebind = template< class U > struct rebind { typedef allocator<U> other; };
	using rebind = struct rebind { typedef Allocator<T> other; };

	using is_always_equal = std::true_type;
private:

	class chunk {
	private:
		class memory {
		public:
			memory(char* ptr, size_t count, memory* next = NULL) :ptr(ptr), count(count), next(next) {}
			char* ptr;
			size_t count;
			memory* next;
		};
		memory* busy;
	public:
		char* data;
		chunk* next;
		chunk(chunk* next = NULL) : next(next) { data = new char[N * sizeof(T)]; }
		~chunk() {
			memory* temp = busy;
			while (temp != NULL) {
				memory* del = temp;
				temp = temp->next;
				delete del;
			}
			busy = NULL;
			next = NULL;
			delete[] data;
			data = NULL;
		}

		char* get(size_t size) {
			if (busy != NULL) { //если что-то уже занято
				if (busy->ptr - data >= size) { //есть ли до первого занятого есть место
					busy->next = new memory(data, size); //если да, то добавляем
					return busy->next->ptr;
				}

				if (busy->next == NULL) //если элемент только один
					if (data + N - (busy->ptr + busy->count) >= size) {  //проверяем останется ли место после него
						busy->next = new memory(busy->ptr + busy->count, size); //если да, то добавляем
						return busy->next->ptr;
					}
					else
						return NULL; //места нет нигде (до или после) - при условии одного элемента

				memory* p1 = busy;
				memory* p2 = busy->next;

				while (p2 != NULL) { //пока не переберём все пары p1-p2
					if (p2->ptr - (p1->ptr + p1->count) >= size) { //если есть место-то между занятыми
						p1->next = new memory(p1->ptr + p1->count, size, p2); //то добавляем
						return p1->next->ptr;
					}
					p1 = p2;
					p2 = p2->next;
				}

				if (data + N - (p1->ptr + p1->count) >= size) {  //проверяем останется ли место после последнего (p1)
					p1->next = new memory(p1->ptr + p1->count, size); //если да, то добавляем
					return p1->next->ptr;
				}

				return NULL; //если ничего не нашли
			}
			else {
				busy = new memory(data, size, NULL);
				return busy->ptr;
			}
		}

		bool del(T* ptr) {
			if (busy != NULL) {
				if (busy->ptr == (char*) ptr) {
					memory* del = busy;
					busy = busy->next;
					delete del;
					return true;
				}

				memory* temp = busy;
				while (temp->next != NULL)
					if (temp->next->ptr == (char*)ptr) {
						memory* del = temp->next;
						temp->next = del->next;
						delete del;
						return true;
					}
			}
			return false;
		}

		static const size_t N = 100;
	};

public:
	Allocator() {
		tail = NULL;
		count = new size_t;
		*count = 0;
	}

	Allocator(const Allocator& B) {
		tail = B.tail;
		count = B.count;
		(*count)++;
	}

	~Allocator() {
		if (*count > 1)
			(*count)--;
		else
		{
			chunk* temp = tail;
			while (temp != NULL) {
				chunk* del = temp;
				temp = temp->next;
				delete del;
			}
			tail = NULL;
			delete[] count;
			count = NULL;
		}
	}

	T* allocate(size_t size) {
		if (size <= chunk::N)
		{
			T* ptr = NULL;
			tail = tail != NULL ? tail : (new chunk); //выделение памяти под первый
			chunk* temp = tail;
			do {
				ptr = (T*)temp->get(size);
				temp = temp->next;
			} while (ptr == NULL && temp != NULL); //пока в имеющихся чанках не будет найдено простанство или они не кончатся

			if (temp == NULL && ptr == NULL) { //если чанки кончились и место не найдено, то добавляем новый чанк
				tail = new chunk(tail);
				ptr = (T*)tail->get(size);
			}

			return ptr;
		}
		return NULL;
	}

	//надо ли использовать размер?
	void deallocate(T* ptr, size_t size) {
		if (tail != NULL) {
			chunk* temp = tail;
			while (temp != NULL && !temp->del(ptr))
				temp = temp->next;
		}
	}

	static size_t max_size() { return chunk::N; }

	Allocator& operator =(const Allocator& B) {
		this->~Allocator();
		tail = B.tail;
		(*B.count)++;
		count = B.count;
		return *this;
	}

	//ругается на шаблон - не могу понять
	/*<template typename ...Args>
	void construct(T* ptr, const Args&&... args) {
		return new(ptr) T(args...);
	};*/
	void construct(T* ptr) {
		return new(ptr) T();
	};

	void destroy(T* ptr) {
		ptr->~T();
	};

private:

	size_t* count;
	chunk* tail;
};

int main()
{
	Allocator<int> alloc;
	Allocator<int> pa;
	int* p = alloc.allocate(5);
	alloc.deallocate(p, 5);
	pa = alloc;
	Allocator<int> alloc2(pa);
	alloc.max_size();
	return 0;
}
