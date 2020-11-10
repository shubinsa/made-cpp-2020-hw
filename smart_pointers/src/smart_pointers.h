#pragma once

#include <algorithm>

namespace task {

	template <class T>
	class WeakPtr;

	template <class T>
	class SharedPtr;

	template <class T>
	struct ControlBlock {
		ControlBlock() : sh_count(1), w_count(0) {};
		size_t sh_count;
		size_t w_count;
	};


	template <class T>
	class UniquePtr {
	public:
		UniquePtr(T* ptr) :ptr(ptr) {}
		UniquePtr(const UniquePtr<T>&) = delete;
		UniquePtr(UniquePtr<T>&& other) : ptr(other.ptr) { other.ptr = nullptr; }
		T* operator->() { return ptr; }
		T& operator*() { return *ptr; }
		const T& operator*() const { return *ptr; }
		UniquePtr<T>& operator=(const UniquePtr<T>& other) = delete;
		UniquePtr<T>& operator=(UniquePtr<T>&& other);

		T* release();
		T* get() { return ptr; }
		void reset();
		void swap(UniquePtr<T>& other);

		~UniquePtr() { delete ptr; }
	private:
		T* ptr;
	};

	template <class T>
	UniquePtr<T>& UniquePtr<T>::operator=(UniquePtr<T>&& other) {
		delete(ptr);
		ptr = other.ptr;
		other.ptr = nullptr;
		return *this;
	}

	template <class T>
	T* UniquePtr<T>::release() {
		T* copy = ptr;
		ptr = nullptr;
		return copy;
	}

	template <class T>
	void UniquePtr<T>::reset() {
		delete ptr;
		ptr = nullptr;
	}

	template <class T>
	void UniquePtr<T>::swap(UniquePtr<T>& other) {
		std::swap(ptr, other.ptr);
	}

	//SharedPtr


	template <class T>
	class SharedPtr {
	public:
		SharedPtr() :CB(), ptr(nullptr) {};
		SharedPtr(T* ptr);
		SharedPtr(const SharedPtr<T>&);
		SharedPtr(SharedPtr<T>&&);
		SharedPtr(const WeakPtr<T>&);
		SharedPtr<T>& operator=(const SharedPtr<T>& other);
		SharedPtr<T>& operator=(SharedPtr<T>&& other);
		T* operator->() { return ptr; }
		T& operator*() { return *(ptr); }
		const T& operator*() const { return *(ptr); }

		void reset();
		void reset(T* ptr);
		void swap(SharedPtr<T>& other);
		T* get() { return (ptr); }

		size_t use_count();

		~SharedPtr();

		friend WeakPtr<T>;
	private:
		ControlBlock<T>* CB;
		T* ptr;
	};

	template <class T>
	SharedPtr<T>::SharedPtr(T* ptr) :ptr(ptr) {
		CB = new ControlBlock<T>;
	}

	template <class T>
	SharedPtr<T>::SharedPtr(const SharedPtr<T>& other) : CB(other.CB) , ptr(other.ptr) {
		if (CB)
			++(CB->sh_count);
	}

	template <class T>
	SharedPtr<T>::SharedPtr(SharedPtr<T>&& other) : CB(other.CB), ptr(other.ptr) {
		other.CB = nullptr;
		other.ptr = nullptr;
	}

	template <class T>
	SharedPtr<T>::SharedPtr(const WeakPtr<T>& weak) : CB(weak.CB), ptr(weak.ptr) {
		if (ptr) 
			++(CB->sh_count);

	}

	template <class T>
	SharedPtr<T>::~SharedPtr() {
		if (CB) {
			--(CB->sh_count);
			if (CB->sh_count == 0) {
				delete ptr;
				ptr = nullptr;
				if (CB->w_count == 0) {
					delete CB;
					CB = nullptr;
				}
			}
		}
	}

	template <class T>
	SharedPtr<T>& SharedPtr<T>::operator=(const SharedPtr<T>& other) {
		if (this != &other) {
			this->~SharedPtr();

			CB = other.CB;
			ptr = other.ptr;
			if (CB) 
				++(CB->sh_count);
		}

		return *this;
	}

	template <class T>
	SharedPtr<T>& SharedPtr<T>::operator=(SharedPtr<T>&& other) {
		this->~SharedPtr();

		CB = other.CB;
		ptr = other.ptr;

		other.CB = nullptr;
		other.ptr = nullptr;

		return *this;
	}

	template <class T>
	void SharedPtr<T>::reset() {
		this->~SharedPtr();
	}

	template <class T>
	void SharedPtr<T>::reset(T* ptr) {
		this->~SharedPtr();
		SharedPtr<T>::ptr = ptr;
		CB = new ControlBlock<T>;
	}

	template <class T>
	void SharedPtr<T>::swap(SharedPtr<T>& other) {
		std::swap(CB, other.CB);
		std::swap(ptr, other.ptr);
	}

	template <class T>
	size_t SharedPtr<T>::use_count() {
		return (CB ? CB->sh_count : 0);
	}


	//WeakPtr


	template <class T>
	class WeakPtr {
	public:
		WeakPtr():CB(nullptr), ptr(nullptr) {};
		WeakPtr(const SharedPtr<T>& shared);
		WeakPtr(const WeakPtr<T>& other);
		WeakPtr(WeakPtr<T>&& other);
		WeakPtr<T>& operator=(WeakPtr<T>&&);
		WeakPtr<T>& operator=(const WeakPtr<T>& other);
		WeakPtr<T>& operator=(const SharedPtr<T>& shared);

		size_t use_count();
		bool expired();

		SharedPtr<T> lock();

		void reset();
		void swap(WeakPtr& other);
		T* get() { return CB->ptr; }

		~WeakPtr();
		friend SharedPtr<T>;
	private:
		ControlBlock<T>* CB;
		T* ptr;
	};


	template <class T>
	WeakPtr<T>::WeakPtr(const WeakPtr<T>& other) :CB(other.CB), ptr(other.ptr) {
		if (CB)
			++(CB->w_count);
	}

	template <class T>
	WeakPtr<T>::WeakPtr(const SharedPtr<T>& shared) :CB(shared.CB), ptr(shared.ptr) {
		if (CB)
			++(CB->w_count);
	}

	template <class T>
	WeakPtr<T>::WeakPtr(WeakPtr<T>&& other) : CB(other.CB), ptr(other.ptr) {
		other.CB = nullptr;
		other.ptr = nullptr;
	}

	template <class T>
	WeakPtr<T>& WeakPtr<T>::operator=(WeakPtr<T>&& other) {
		this->~WeakPtr();
		CB = other.CB;
		ptr = other.ptr;
		other.CB = nullptr;
		other.ptr = nullptr;
	}

	template <class T>
	WeakPtr<T>& WeakPtr<T>::operator=(const WeakPtr<T>& other) {
		if (this != &other) {
			this->~WeakPtr();

			CB = other.CB;
			
			if (CB) {
				++(CB->w_count);
				ptr = other.ptr;
			}
		}

		return *this;
	}

	template <class T>
	WeakPtr<T>& WeakPtr<T>::operator=(const SharedPtr<T>& shared) {
		this->~WeakPtr();
		CB = shared.CB;
		
		if (CB) {
			++(CB->w_count);
			ptr = shared.ptr;
		}
		return *this;
	}

	template <class T>
	size_t WeakPtr<T>::use_count() {
		return (CB ? CB->sh_count : 0);
	}

	template <class T>
	bool WeakPtr<T>::expired() {
		return (CB ? (!CB->sh_count) : 0);
	}

	template <class T>
	SharedPtr<T> WeakPtr<T>::lock() {
		return (CB->sh_count == 0 ? SharedPtr<T>() : SharedPtr<T>(*this));
	}

	template <class T>
	void WeakPtr<T>::reset() {
		this->~WeakPtr();
	}

	template <class T>
	void WeakPtr<T>::swap(WeakPtr& other) {
		std::swap(CB, other.CB);
		std::swap(ptr, other.ptr);
	}

	template <class T>
	WeakPtr<T>::~WeakPtr() {
		if (CB) {
			--(CB->w_count);
			if (CB->sh_count == 0 && CB->w_count == 0) {
				delete CB;
				CB = nullptr;
			}
		}
	}


}  // namespace task


//#include "smart_pointers.tpp"