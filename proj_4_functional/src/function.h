#ifndef __FUNCTION
#define __FUNCTION

#include <utility>
#include <memory>

template <class Unused>
class function;

template <class Unused>
struct callableBase;

template <class ReturnType, class ...Args>
struct callableBase<ReturnType(Args...)> {
	virtual ReturnType operator()(Args... args) = 0;
	virtual std::unique_ptr <callableBase> clone() = 0;
	virtual void* getFunctorPtr() = 0;
	virtual const std::type_info& typeID() = 0;
};

template <typename F, class ReturnType, class ...Args>
struct callable : callableBase<ReturnType(Args...)> {
	typedef F functor_type;
	virtual std::unique_ptr<callableBase<ReturnType(Args...)>> clone() override {
		return std::make_unique<callable>(functor);
	}
	virtual void* getFunctorPtr() override {
		return reinterpret_cast<void*>(std::addressof(functor));
	}

	const std::type_info& typeID() override {
		return typeid(F);
	}

	F functor;
	explicit callable(F functor) :functor(functor) {};
	ReturnType operator()(Args... args) override { return functor(args...); }
};

template <class ReturnType, class ...Args>
class ::function<ReturnType(Args...)> {
public:
	typedef ReturnType result_type;
	typedef std::unique_ptr<callableBase<result_type(Args...)>> SmartPtr;
	SmartPtr ptr;

	//CONSTRUCTORS

	function() noexcept :ptr(std::nullptr_t())  {};

	function(std::nullptr_t) noexcept :ptr(std::nullptr_t()) {};

	function(const function& other) {
		ptr = other.ptr->clone();
	};

	function(function&& other) noexcept {
		ptr.swap(other.ptr);
	};

	template<class F>
	function(F f){
		ptr = std::make_unique<callable<F, result_type, Args...>>(std::move(f));
	}

	//DESTRUCTOR

	~function() {
		ptr.reset(std::nullptr_t());
	};

	//OTHER METHODS

	function operator=(const function& other) {
		if (&other != this) {
			ptr.~unique_ptr();
			ptr = std::make_unique<callableBase<ReturnType(Args)>>(*(other.ptr));
		}
		return *this;
	};

	function operator=(function&& other) noexcept {
		if (&other != this) {
			ptr.swap(other.ptr);
		}
		return *this;
	};

	function& operator=(std::nullptr_t) noexcept {
		ptr.reset(std::nullptr_t());

		return *this;
	}

	void swap(function& other) noexcept {
		SmartPtr temp = std::move(ptr);
		ptr = std::move(other.ptr);
		other.ptr = std::move(temp);
	}

	template<class F>
	function& operator=(F&& f) {
		function(std::forward<F>(f)).swap(*this);
		return *this;
	}

	template< class F >
	function& operator=(std::reference_wrapper<F> f) noexcept {
		function(f).swap(*this);
		return *this;
	}

	template<class F, class Alloc>
	void assign(F&& other, const Alloc& alloc) {
		function(std::allocator_arg, alloc, std::forward<F>(other)).swap(*this);
	}

	explicit operator bool() const {
		return (ptr.get() != NULL) ? true : false;
	};

	ReturnType operator()(Args... arg) const {
		if (!*this == true) 
			throw std::exception("bad_function_call");
		return ptr->operator()(arg...);
	}

	const std::type_info& target_type() const noexcept {
		return (ptr.get() != NULL) ? ptr->typeID() : typeid(void);
	}

	template<class T>
	T* target() noexcept {
		return (target_type() == typeid(T)) ? reinterpret_cast<T*>(ptr->getFunctorPtr()) : NULL;
	}

	template<class T>
	const T* target() const noexcept {
		return (target_type() == typeid(T)) ? reinterpret_cast<T*>(ptr->getFunctorPtr()) : NULL;
	}
};

template<class R, class... Args>
bool operator==(const function<R(Args...)>& f, std::nullptr_t) noexcept {
	return !f;
}

template<class R, class... Args>
bool operator==(std::nullptr_t, const function<R(Args...)>& f) noexcept {
	return !f;
}

template< class R, class... Args >
bool operator!=(const function<R(Args...)>& f, std::nullptr_t) noexcept {
	return (bool)f;
}

template< class R, class... Args >
bool operator!=(std::nullptr_t, const function<R(Args...)>& f) noexcept {
	return (bool)f;
}

template< class R, class... Args >
void swap(function<R(Args...)>& a, function<R(Args...)>& b) noexcept {
	a.swap(b);
}

#endif