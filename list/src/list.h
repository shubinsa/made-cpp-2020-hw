#pragma once
#include <iostream>
#include <iterator>
#include <utility>
#include <cassert>
#include <tuple>

namespace task {
	template<class T, class Alloc = std::allocator<T>>
	class list {
	public:
		struct node {
			node(node* prev, node* next) :value(), prev(prev), next(next) {};
			node(node* prev, node* next, const T& value) : value(value), prev(prev), next(next) {};
			node(node* prev, node* next, T&& value) : value(std::forward<T>(value)), prev(prev), next(next) {};

			template< class... Args>
			node(node* prev, node* next, Args&&... args) :
				prev(prev), next(next), value(std::forward<Args>(args)...) {};

			/*template< class... Args >
			node(node* prev, node* next, std::tuple<Args&&...> args) :
				prev(prev), next(next), value(args) {};
				*/

			T value;
			node* next;
			node* prev;
		};

		class iterator {
		public:
			using difference_type = ptrdiff_t;
			using value_type = T;
			using pointer = T*;
			using reference = T&;
			using iterator_category = std::bidirectional_iterator_tag;

			iterator();
			iterator(const iterator&);
			iterator& operator=(const iterator& other);

			iterator& operator++();
			iterator operator++(int);
			reference operator*() const;
			pointer operator->() const;
			iterator& operator--();
			iterator operator--(int);

			bool operator==(iterator other) const;
			bool operator!=(iterator other) const;

			friend list;
		private:
			iterator(node* elem, list* li);
			node* ptr;
			list* li;
		};

		class const_iterator {
		public:
			using difference_type = ptrdiff_t;
			using value_type = T;
			using pointer = const T*;
			using reference = const T&;
			using iterator_category = std::bidirectional_iterator_tag;

			const_iterator();
			const_iterator(const const_iterator&);
			const_iterator(const iterator&);
			const_iterator& operator=(const const_iterator& other);

			const_iterator& operator++();
			const_iterator operator++(int);
			reference operator*() const;
			pointer operator->() const;
			const_iterator& operator--();
			const_iterator operator--(int);

			bool operator==(const_iterator other) const;
			bool operator!=(const_iterator other) const;

			friend list;
		private:
			const_iterator(node* elem, list* li);
			node* ptr;
			list* li;
		};

		using reverse_iterator = std::reverse_iterator<iterator>;
		using const_reverse_iterator = std::reverse_iterator<const_iterator>;


		list();
		explicit list(const Alloc& alloc);
		list(size_t count, const T& value, const Alloc& alloc = Alloc());
		explicit list(size_t count, const Alloc& alloc = Alloc());

		~list();

		list(const list& other);
		list(list&& other);
		list& operator=(const list& other);
		list& operator=(list&& other);

		Alloc get_allocator() const;


		T& front();
		const T& front() const;

		T& back();
		const T& back() const;


		iterator begin();
		iterator end();

		const_iterator cbegin() const;
		const_iterator cend() const;

		reverse_iterator rbegin();
		reverse_iterator rend();

		const_reverse_iterator crbegin() const;
		const_reverse_iterator crend() const;


		bool empty() const;
		size_t size() const;
		size_t max_size() const;
		void clear();

		iterator insert(const_iterator pos, const T& value);
		iterator insert(const_iterator pos, T&& value);
		iterator insert(const_iterator pos, size_t count, const T& value);

		iterator erase(const_iterator pos);
		iterator erase(const_iterator first, const_iterator last);


		void push_back(const T& value);
		void push_back(T&& value);
		void pop_back();

		void push_front(const T& value);
		void push_front(T&& value);
		void pop_front();

		template <class... Args>
		iterator emplace(const_iterator pos, Args&&... args);

		template <class... Args>
		void emplace_back(Args&&... args);

		template <class... Args>
		void emplace_front(Args&&... args);

		void resize(size_t count);
		void swap(list& other);


		void merge(list& other);
		void splice(const_iterator pos, list& other);
		void remove(const T& value);
		void reverse();
		void unique();
		void sort();

	private:
		void copy(const list& other);
		std::pair<node*, node*> copy(size_t count, const T& value) const;
		std::pair<node*, node*> copy(size_t count) const;

		//using AllocNode = typename Alloc::template rebind<node>::other;
		mutable typename Alloc::template rebind<node>::other alloc;

		size_t ssize;

		node* head;
		node* tail;
	};




	template<class T, class Alloc>
	list<T, Alloc>::list() : head(NULL), tail(NULL), ssize(0) {}

	template<class T, class Alloc>
	list<T, Alloc>::list(const Alloc& alloc) : head(NULL), tail(NULL), ssize(0), alloc(alloc) {}

	template<class T, class Alloc>
	list<T, Alloc>::list(size_t count, const T& value, const Alloc& alloc) : alloc(alloc), ssize(count) {
		auto pair = copy(count, value);
		head = pair.first;
		tail = pair.second;
	}

	template<class T, class Alloc>
	list<T, Alloc>::list(size_t count, const Alloc& alloc) : alloc(alloc), ssize(count) {
		auto pair = copy(count);
		head = pair.first;
		tail = pair.second;
	}

	template<class T, class Alloc>
	list<T, Alloc>::~list() {
		clear();
	}

	template<class T, class Alloc>
	list<T, Alloc>::list(const list& other) {
		copy(other);
	}

	template<class T, class Alloc>
	list<T, Alloc>::list(list&& other) :head(other.head), tail(other.tail), ssize(other.ssize) {
		other.head = NULL;
		other.tail = NULL;
		other.ssize = 0;
		std::swap(other.alloc, alloc);
	}

	template<class T, class Alloc>
	list<T, Alloc>& list<T, Alloc>::operator=(const list& other) {
		if (&other != this) {
			clear();
			copy(other);
			ssize = other.ssize;
		}
		return *this;
	}

	template<class T, class Alloc>
	list<T, Alloc>& list<T, Alloc>::operator=(list&& other) {
		std::swap(other.head, head);
		std::swap(other.tail, tail);
		std::swap(other.ssize, ssize);
		std::swap(other.alloc, alloc);
		return *this;
	}

	template<class T, class Alloc>
	void list<T, Alloc>::copy(const list& other) {
		if (other.head == NULL)
			head = tail = NULL;
		else {
			head = alloc.allocate(1);
			alloc.construct(head, static_cast<node*>(NULL), static_cast<node*>(NULL), other.head->value);
			node* last = head;
			for (node* i = other.head->next; i != NULL; i = i->next) { //проход по other
				last->next = alloc.allocate(1);
				alloc.construct(last->next, last, static_cast<node*>(NULL), i->value);	//значение + ссылка вперёд
				last = last->next;								//обновляем последний
			}
			tail = last;
			ssize = other.ssize;

		}
	}


	template<class T, class Alloc>
	Alloc list<T, Alloc>::get_allocator() const {
		return alloc;
	}

	template<class T, class Alloc>
	T& list<T, Alloc>::front() {
		assert(head && "front() called on empty list");
		return head->value;
	}

	template<class T, class Alloc>
	const T& list<T, Alloc>::front() const {
		assert(head && "front() called on empty list");
		return head->value;
	}

	template<class T, class Alloc>
	T& list<T, Alloc>::back() {
		assert(tail && "back() called on empty list");
		return tail->value;
	}

	template<class T, class Alloc>
	const T& list<T, Alloc>::back() const {
		assert(tail && "back() called on empty list");
		return tail->value;
	}

	template<class T, class Alloc>
	bool list<T, Alloc>::empty() const {
		return !ssize;
	}

	template<class T, class Alloc>
	size_t list<T, Alloc>::size() const {
		return ssize;
	}

	template<class T, class Alloc>
	size_t list<T, Alloc>::max_size() const {
		alloc.max_size();
	}

	template<class T, class Alloc>
	void list<T, Alloc>::clear() {
		node* i = head;
		while (i != NULL) {
			node* del = i;
			i = i->next;
			alloc.deallocate(del, 1);
		}
		head = NULL;
		tail = NULL;
		ssize = 0;
	}

	template<class T, class Alloc>
	typename list<T, Alloc>::iterator  list<T, Alloc>::insert(const_iterator pos, const T& value) {
		node* p;
		if (pos.ptr == NULL) {			//вставка в конец
			p = alloc.allocate(1);
			if (tail == NULL) {			//если список пуст, обновляем всё
				alloc.construct(p, static_cast<node*>(NULL), static_cast<node*>(NULL), value);
				head = tail = p;
			}
			else {						//если список не пуст, вставляем в конец
				tail->next = p;
				alloc.construct(p, tail, static_cast<node*>(NULL), value);
				tail = p;
			}
		}
		else
			if (head == pos.ptr) {		//если вставка в начало
				p = alloc.allocate(1);
				alloc.construct(p, static_cast<node*>(NULL), head, value);	//значение + ссылка вперёд
				head->prev = p;
				head = p;
			}
			else {
				p = alloc.allocate(1);
				alloc.construct(p, pos.ptr->prev, pos.ptr, value);	//значение + ссылка вперёд
				p->prev->next = p;	//предыдущий указывает на новый
				p->next->prev = p;	//следующий тоже
			}
		++ssize;

		return iterator(p, this);
	}

	template<class T, class Alloc>
	typename list<T, Alloc>::iterator  list<T, Alloc>::insert(const_iterator pos, T&& value) {
		node* p;
		if (pos.ptr == NULL) {			//вставка в конец
			p = alloc.allocate(1);
			if (tail == NULL) {			//если список пуст, обновляем всё
				alloc.construct(p, static_cast<node*>(NULL), static_cast<node*>(NULL), std::forward<T>(value));
				head = tail = p;
			}
			else {						//если список не пуст, вставляем в конец
				tail->next = p;
				alloc.construct(p, tail, static_cast<node*>(NULL), std::forward<T>(value));
				tail = p;
			}
		}
		else
			if (head == pos.ptr) {		//если вставка в начало
				p = alloc.allocate(1);
				alloc.construct(p, static_cast<node*>(NULL), head, value);	//значение + ссылка вперёд
				head->prev = p;
				head = p;
			}
			else {
				p = alloc.allocate(1);
				alloc.construct(p, pos.ptr->prev, pos.ptr, std::forward<T>(value));	//значение + ссылка вперёд
				p->prev->next = p;	//предыдущий указывает на новый
				p->next->prev = p;	//следующий тоже
			}
		++ssize;

		return iterator(p, this);
	}

	template<class T, class Alloc>
	typename list<T, Alloc>::iterator  list<T, Alloc>::insert(const_iterator pos, size_t count, const T& value) {
		auto pair = copy(count, value);

		if (pos.ptr == NULL)
			if (tail == NULL) {
				head = pair.first;
				tail = pair.second;
			}
			else {
				tail->next = pair.first;
				pair.first->prev = tail;
				tail = pair.second;
			}
		else
			if (head == pos.ptr) {		//если вставка в начало
				pair.second->next = head;		//сцепляем конец c головой
				head->prev = pair.second;
				head = pair.first;
			}
			else {
				pair.first->prev = pos.ptr->prev;	//сцепляем начало списка
				pair.second->next = pos.ptr;		//сцепляем конец

				pair.first->prev->next = pair.first;	//предыдущий указывает на новый список (вставленный)
				pair.second->next->prev = pair.second;	//следующий тоже указывает на него
			}

		ssize += count;

		return iterator(pair.first, this);
	}

	template<class T, class Alloc>
	std::pair<typename list<T, Alloc>::node*, typename list<T, Alloc>::node*>
		list<T, Alloc>::copy(size_t count, const T& value) const {
		node* chead;
		node* fictive = alloc.allocate(1);
		node* last = fictive;
		for (size_t i = 0; i < count; i++) {
			last->next = alloc.allocate(1);
			alloc.construct(last->next, last, static_cast<node*>(NULL), value);	//значение + ссылка вперёд
			last = last->next;							//обновляем последний
		}
		chead = fictive->next;
		alloc.deallocate(fictive, 1);

		return { chead, last };
	}

	template<class T, class Alloc>
	std::pair<typename list<T, Alloc>::node*, typename list<T, Alloc>::node*>
		list<T, Alloc>::copy(size_t count) const {
		node* chead;
		node* fictive = alloc.allocate(1);
		node* last = fictive;
		for (size_t i = 0; i < count; i++) {
			last->next = alloc.allocate(1);
			alloc.construct(last->next, last, static_cast<node*>(NULL));		//значение по умолчанию + ссылка вперёд
			last = last->next;						//обновляем последний
		}
		chead = fictive->next;
		alloc.deallocate(fictive, 1);

		return { chead, last };
	}

	template<class T, class Alloc>
	typename list<T, Alloc>::iterator list<T, Alloc>::erase(const_iterator pos) {
		if (pos != NULL) {
			node* p = pos.ptr;
			node* next = p->next;

			if (head == p) {
				head = head->next;
				head->prev = NULL;
			}
			else
				p->prev->next = p->next;

			if (tail == p) {
				tail = tail->prev;
				tail->next = NULL;
			}
			else
				p->next->prev = p->prev;

			alloc.deallocate(p, 1);
			--ssize;

			return iterator(next, this);
		}
		return iterator(NULL, this);
	}

	template<class T, class Alloc>
	typename list<T, Alloc>::iterator list<T, Alloc>::erase(const_iterator first, const_iterator last) {
		node* f = first.ptr;
		node* l = last.ptr;

		if (head == f)
			head = l;			//если удаляем из начала списка
		else
			f->prev->next = l;

		if (l == NULL)				//если удаляем из конца
			tail = f->prev;
		else
			l->prev = f->prev;

		node* i = f;
		while (i != l) {
			node* del = i;
			i = i->next;
			--ssize;
			alloc.deallocate(del, 1);
		}

		return iterator(l, this); //либо last, либо next
	}

	template<class T, class Alloc>
	void list<T, Alloc>::push_back(const T& value) {
		node* p = alloc.allocate(1);
		if (tail == NULL) {			//если список пуст, обновляем всё
			alloc.construct(p, static_cast<node*>(NULL), static_cast<node*>(NULL), value);
			head = tail = p;
		}
		else {						//если список не пуст, вставляем в конец
			tail->next = p;
			alloc.construct(p, tail, static_cast<node*>(NULL), value);
			tail = p;
		}
		++ssize;
	}

	template<class T, class Alloc>
	void list<T, Alloc>::push_back(T&& value) {
		node* p = alloc.allocate(1);
		if (tail == NULL) {			//если список пуст, обновляем всё
			alloc.construct(p, static_cast<node*>(NULL), static_cast<node*>(NULL), std::forward<T>(value));
			head = tail = p;
		}
		else {						//если список не пуст, вставляем в конец
			tail->next = p;
			alloc.construct(p, tail, static_cast<node*>(NULL), std::forward<T>(value));
			tail = p;
		}
		++ssize;
	}

	template<class T, class Alloc>
	void list<T, Alloc>::pop_back() {
		assert(tail && "pop_back() called on empty list");

		node* p = tail;
		if (head != tail) {
			tail->prev->next = NULL;
			tail = tail->prev;
		}
		else
			head = tail = NULL;

		alloc.deallocate(p, 1);
		--ssize;
	}

	template<class T, class Alloc>
	void list<T, Alloc>::push_front(const T& value) {
		node* p = alloc.allocate(1);
		if (head == NULL) {			//если список пуст, обновляем всё
			alloc.construct(p, static_cast<node*>(NULL), static_cast<node*>(NULL), value);
			head = tail = p;
		}
		else {						//если список не пуст, вставляем в начало
			head->prev = p;
			alloc.construct(p, static_cast<node*>(NULL), head, value);
			head = p;
		}
		++ssize;
	}

	template<class T, class Alloc>
	void list<T, Alloc>::push_front(T&& value) {
		node* p = alloc.allocate(1);
		if (head == NULL) {			//если список пуст, обновляем всё
			alloc.construct(p, static_cast<node*>(NULL), static_cast<node*>(NULL), std::forward<T>(value));
			head = tail = p;
		}
		else {						//если список не пуст, вставляем в начало
			head->prev = p;
			alloc.construct(p, static_cast<node*>(NULL), head, std::forward<T>(value));
			head = p;
		}
		++ssize;
	}

	template<class T, class Alloc>
	void list<T, Alloc>::pop_front() {
		assert(head && "pop_front() called on empty list");

		node* p = head;
		if (head != tail) {
			head->next->prev = NULL;
			head = head->next;
		}
		else
			head = tail = NULL;

		alloc.deallocate(p, 1);
		--ssize;
	}

	template<class T, class Alloc>
	template <class... Args>
	typename list<T, Alloc>::iterator list<T, Alloc>::emplace(const_iterator pos, Args&&... args) {
		node* p;
		if (pos.ptr == NULL) {			//вставка в конец
			p = alloc.allocate(1);
			if (tail == NULL) {			//если список пуст, обновляем всё
				alloc.construct(p, static_cast<node*>(NULL), static_cast<node*>(NULL), std::forward<Args>(args)...); //forward_as_tuple - убрано
				head = tail = p;
			}
			else {						//если список не пуст, вставляем в конец
				tail->next = p;
				//alloc.construct(p, tail, static_cast<node*>(NULL), std::forward_as_tuple(std::forward<Args>(args)...));
				alloc.construct(p, tail, static_cast<node*>(NULL), std::forward<Args>(args)...);
				tail = p;
			}
		}
		else {
			p = alloc.allocate(1);
			alloc.construct(p, pos.ptr->prev, pos.ptr, std::forward<Args>(args)...);	//значение + ссылка вперёд
			p->prev->next = p;	//предыдущий указывает на новый
			p->next->prev = p;	//следующий тоже
		}
		++ssize;

		return iterator(p, this);
	}

	template<class T, class Alloc>
	template <class... Args>
	void list<T, Alloc>::emplace_back(Args&&... args) {
		node* p = alloc.allocate(1);
		if (tail == NULL) {			//если список пуст, обновляем всё
			//alloc.construct(p, static_cast<node*>(NULL), static_cast<node*>(NULL), std::forward_as_tuple(std::forward<Args>(args)...)); 

			alloc.construct(p, static_cast<node*>(NULL), static_cast<node*>(NULL), std::forward<Args>(args)...);

			head = tail = p;
		}
		else {						//если список не пуст, вставляем в конец
			tail->next = p;
			//std::forward<Args>(args)...
			//std::forward<Args>(args)
			//std::forward<Args&&>(args)

			alloc.construct(p, tail, static_cast<node*>(NULL), std::forward<Args>(args)...);
			tail = p;
		}
		++ssize;

	}

	template<class T, class Alloc>
	template <class... Args>
	void list<T, Alloc>::emplace_front(Args&&... args) {
		node* p = alloc.allocate(1);
		if (head == NULL) {			//если список пуст, обновляем всё
			alloc.construct(p, static_cast<node*>(NULL), static_cast<node*>(NULL), std::forward<Args>(args)...);
			head = tail = p;
		}
		else {						//если список не пуст, вставляем в начало
			head->prev = p;
			alloc.construct(p, static_cast<node*>(NULL), head, std::forward<Args>(args)...);
			head = p;
		}
		++ssize;
	}


	template<class T, class Alloc>
	void list<T, Alloc>::resize(size_t count) {
		if (count < ssize) {
			size_t i;
			node* n;
			for (i = 0, n = head; i < count; i++, n = n->next); //переход к первому удаляемому

			if (n != head)
				n->prev->next = NULL;			//обрубаем список (по удаляемому)
			else
				head = NULL;

			tail = n->prev;

			while (n != NULL) {	//удаляем лишние
				node* del = n;
				n = n->next;
				alloc.deallocate(del, 1);
			}
		}
		else
			if (count > ssize) {
				auto pair = copy(count - ssize);

				if (tail != NULL) {
					tail->next = pair.first;
					pair.first->prev = tail;
				}
				else
					head = pair.first;

				tail = pair.second;
			}
		ssize = count;
	}

	template<class T, class Alloc>
	void list<T, Alloc>::swap(list& other) {
		std::swap(other.head, head);
		std::swap(other.tail, tail);
		std::swap(other.ssize, ssize);
		std::swap(other.alloc, alloc);
	}

	template<class T, class Alloc>
	void list<T, Alloc>::merge(list& other) {
		if (&other != this) {
			node* fictive = alloc.allocate(1);
			node* last = fictive;
			node* p1 = head, * p2 = other.head;

			while (p1 != NULL && p2 != NULL)
				if (p1->value < p2->value) {
					last->next = p1;
					p1->prev = last;
					last = last->next;
					p1 = p1->next;
				}
				else
					if (p1->value > p2->value) {
						last->next = p2;
						p2->prev = last;
						last = last->next;
						p2 = p2->next;
					}
					else {
						p1->prev = last;
						last->next = p1;
						node* next = p1->next;
						p1->next = p2;
						p2->prev = p1;
						last = p2;

						p1 = next;
						p2 = p2->next;
					}

			if (p1 != NULL || p2 != NULL) {			//если ещё что-то не добавили, добавляем и находим tail
				node* rest;
				if (p2 != NULL) {		//если что-то осталось во втором, то будем брать из него и tail тоже
					rest = p2;
					tail = other.tail;
				}
				else
					rest = p1;			//если что-то осталось в первом, берём из него, а tail не меняется

				last->next = rest;
				rest->prev = last;
			}
			else	//если оба списка пусты, то последний добавленный - tail
				tail = last;

			head = fictive->next;
			alloc.deallocate(fictive, 1);
			ssize += other.ssize;

			other.head = other.tail = NULL;
			other.ssize = 0;
		}
	}

	template<class T, class Alloc>
	void list<T, Alloc>::splice(const_iterator pos, list& other) {
		if (pos.ptr == NULL)
			if (tail == NULL) {
				head = other.head;
				tail = other.tail;
			}
			else {
				tail->next = other.head;
				other.head->prev = tail;
				tail = other.tail;
			}
		else {
			other.head->prev = pos.ptr->prev;	//сцепляем начало списка
			other.tail->next = pos.ptr;		//сцепляем конец

			other.head->prev->next = other.head;	//предыдущий указывает на новый список (вставленный)
			other.tail->next->prev = other.tail;	//следующий тоже указывает на него же
		}

		ssize += other.ssize;
		other.head = other.tail = NULL;
		other.ssize = 0;
	}

	template<class T, class Alloc>
	void list<T, Alloc>::remove(const T& value) {
		T val = value; //иначе уничтожается снаружи???
		while (head != NULL && head->value == val) {	//подчищаем головные
			node* del = head;
			head = head->next;
			if (head != NULL)
				head->prev = NULL;
			alloc.deallocate(del, 1);
			--ssize;
		}

		if (head == NULL)		//если всё удалили, то заменяем tail
			tail = NULL;
		else {
			node* i = head->next;
			while (i->next != NULL) {		//перебор всех, кроме головы и хвоста
				if (i->value == val) {
					node* del = i;
					del->prev->next = del->next;
					del->next->prev = del->prev;
					i = i->next;
					alloc.deallocate(del, 1);
					--ssize;
				}
				else
					i = i->next;
			}

			if (tail->value == value) {		//удаление из хвоста, если надо
				node* del = tail;
				tail = tail->prev;
				tail->next = NULL;
				alloc.deallocate(del, 1);
				--ssize;
			}
		}
	}


	template<class T, class Alloc>
	void list<T, Alloc>::reverse() {
		node* i = head;
		while (i != NULL) {
			node* next = i->next;
			std::swap(i->prev, i->next);
			i = next;
		}
		std::swap(head, tail);
	}

	template<class T, class Alloc>
	void list<T, Alloc>::unique() {
		if (head != tail) { //если в списке более одного элемента
			node* i = head->next;
			while (i->next != NULL) {		//проверяем все, между головой и хвостом
				if (i->value == i->prev->value) {
					i->prev->next = i->next;
					i->next->prev = i->prev;
					node* del = i;
					i = i->next;
					alloc.deallocate(del, 1);
					--ssize;
				}
				else
					i = i->next;
			}

			if (tail->value == tail->prev->value) {
				node* del = tail;
				tail = tail->prev;
				tail->next = NULL;
				alloc.deallocate(del, 1);
				--ssize;
			}
		}
	}

	template<class T, class Alloc>
	void list<T, Alloc>::sort() {
		if (head != tail) {
			for (node* i = head; i->next != NULL; i = i->next) {	//перебираем все до предпоследнего

				node* min = i->next;			//следующий за текущим - хранит минимум
				for (node* j = min->next; j != NULL; j = j->next) //перебираем остальные после min (если он не последний)
					if (j->value < min->value)		//обнаружили новый min - обновляем
						min = j;

				if (i->value > min->value)				//если текущий больше минимума
					std::swap(i->value, min->value);	//меняем содержимое местами
				else										//если текущий меньше min
					std::swap(i->next->value, min->value);	//вставляем min сразу после него
			}
		}
	}

	//iterator

	template<class T, class Alloc>
	list<T, Alloc>::iterator::iterator() : ptr(NULL) {};

	template<class T, class Alloc>
	list<T, Alloc>::iterator::iterator(node* elem, list* li) :ptr(elem), li(li) {};

	template<class T, class Alloc>
	list<T, Alloc>::iterator::iterator(const iterator& other) :ptr(other.ptr), li(other.li) {};

	template<class T, class Alloc>
	typename list<T, Alloc>::iterator& list<T, Alloc>::iterator::operator=(const iterator& other) {
		ptr = other.ptr;
		li = other.li;
		return *this;
	}

	template<class T, class Alloc>
	typename list<T, Alloc>::iterator& list<T, Alloc>::iterator::operator++() {
		ptr = ptr->next;
		return *this;
	}

	template<class T, class Alloc>
	typename list<T, Alloc>::iterator list<T, Alloc>::iterator::operator++(int) {
		node* saved = ptr;
		ptr = ptr->next;
		return iterator(saved, li);
	}

	template<class T, class Alloc>
	typename list<T, Alloc>::iterator::reference list<T, Alloc>::iterator::operator*() const {
		return ptr->value;
	}

	template<class T, class Alloc>
	typename list<T, Alloc>::iterator::pointer list<T, Alloc>::iterator::operator->() const {
		return &(ptr->value);
	}

	template<class T, class Alloc>
	typename list<T, Alloc>::iterator& list<T, Alloc>::iterator::operator--() {
		if (ptr == NULL)
			ptr = li->tail;
		else
			ptr = ptr->prev;
		return *this;
	}

	template<class T, class Alloc>
	typename list<T, Alloc>::iterator list<T, Alloc>::iterator::operator--(int) {
		node* saved = ptr;
		if (ptr == NULL)
			ptr = li->tail;
		else
			ptr = ptr->prev;
		return iterator(saved, this);
	}

	template<class T, class Alloc>
	bool list<T, Alloc>::iterator::operator==(iterator other) const {
		return ptr == other.ptr;
	}

	template<class T, class Alloc>
	bool list<T, Alloc>::iterator::operator!=(iterator other) const {
		return ptr != other.ptr;
	}

	//const_iterator

	template<class T, class Alloc>
	list<T, Alloc>::const_iterator::const_iterator() : ptr(NULL) {};

	template<class T, class Alloc>
	list<T, Alloc>::const_iterator::const_iterator(node* elem, list* li) :ptr(elem), li(li) {};

	template<class T, class Alloc>
	list<T, Alloc>::const_iterator::const_iterator(const const_iterator& other) :ptr(other.ptr), li(other.li) {};

	template<class T, class Alloc>
	list<T, Alloc>::const_iterator::const_iterator(const iterator& other) :ptr(other.ptr), li(other.li) {};

	template<class T, class Alloc>
	typename list<T, Alloc>::const_iterator& list<T, Alloc>::const_iterator::operator=(const const_iterator& other) {
		ptr = other.ptr;
		return *this;
	}

	template<class T, class Alloc>
	typename list<T, Alloc>::const_iterator& list<T, Alloc>::const_iterator::operator++() {
		ptr = ptr->next;
		return *this;
	}

	template<class T, class Alloc>
	typename list<T, Alloc>::const_iterator list<T, Alloc>::const_iterator::operator++(int) {
		node* saved = ptr;
		ptr = ptr->next;
		return const_iterator(saved, li);
	}

	template<class T, class Alloc>
	typename list<T, Alloc>::const_iterator::reference list<T, Alloc>::const_iterator::operator*() const {
		return ptr->value;
	}

	template<class T, class Alloc>
	typename list<T, Alloc>::const_iterator::pointer list<T, Alloc>::const_iterator::operator->() const {
		return &(ptr->value);
	}

	template<class T, class Alloc>
	typename list<T, Alloc>::const_iterator& list<T, Alloc>::const_iterator::operator--() {
		if (ptr == NULL)
			ptr = li->tail;
		else
			ptr = ptr->prev;
		return *this;
	}

	template<class T, class Alloc>
	typename list<T, Alloc>::const_iterator list<T, Alloc>::const_iterator::operator--(int) {
		node* saved = ptr;
		if (ptr == NULL)
			ptr = li->tail;
		else
			ptr = ptr->prev;
		return const_iterator(saved, this);
	}

	template<class T, class Alloc>
	bool list<T, Alloc>::const_iterator::operator==(const_iterator other) const {
		return ptr == other.ptr;
	}

	template<class T, class Alloc>
	bool list<T, Alloc>::const_iterator::operator!=(const_iterator other) const {
		return ptr != other.ptr;
	}

	//доступ к итераторам

	template<class T, class Alloc>
	typename list<T, Alloc>::iterator list<T, Alloc>::begin() {
		return iterator(const_cast<node*>(head), const_cast<list*>(this));
	}

	template<class T, class Alloc>
	typename list<T, Alloc>::iterator list<T, Alloc>::end() {
		return iterator(static_cast<node*>(NULL), const_cast<list*>(this));
	}

	template<class T, class Alloc>
	typename list<T, Alloc>::const_iterator list<T, Alloc>::cbegin() const {
		return const_iterator(const_cast<node*>(head), const_cast<list*>(this));
	}

	template<class T, class Alloc>
	typename list<T, Alloc>::const_iterator list<T, Alloc>::cend() const {
		return const_iterator(static_cast<node*>(NULL), const_cast<list*>(this));
	}

	template<class T, class Alloc>
	typename list<T, Alloc>::reverse_iterator list<T, Alloc>::rbegin() {
		return reverse_iterator<iterator>(static_cast<node*>(tail), const_cast<list*>(this));
	}

	template<class T, class Alloc>
	typename list<T, Alloc>::reverse_iterator list<T, Alloc>::rend() {
		return reverse_iterator<iterator>(const_cast<node*>(head), const_cast<list*>(this));
	}

	template<class T, class Alloc>
	typename list<T, Alloc>::const_reverse_iterator list<T, Alloc>::crbegin() const {
		return const_reverse_iterator(const_iterator(static_cast<node*>(tail->next), const_cast<list*>(this)));
	}

	template<class T, class Alloc>
	typename list<T, Alloc>::const_reverse_iterator list<T, Alloc>::crend() const {
		return const_reverse_iterator(const_iterator(static_cast<node*>(head), const_cast<list*>(this)));
	}

}  // namespace task