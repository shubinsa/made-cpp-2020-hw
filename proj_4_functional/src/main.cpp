#include <iostream>
#include <cassert>

#include "function.h"
#include "compose.h"

int f() { return 1; }
int g() { return 2; }
int u() { return 2; }

char test(function<int()> const& arg)
{
	int (* const* ptr)() = arg.target<int(*)()>();
	if (ptr && *ptr == f)
		return 'f';
	if (ptr && *ptr == g)
		return 'g';

	return '0';
}

double m(double d, int i) {
	return d * i;
}

struct str {
	void operator()(int& i) {
		i = i - 1;
	}
};

int min_func(int a, int b) {
	return a < b ? a : b;
}

int sum_func(int a, int b) {
	return a + b;
}

int dif_func(int a, int b) {
	return a - b;
}

int main()
{
	{
		//constructors & operator()
		function<int(int, int)> max = [](int a, int b) -> int {return a > b ? a : b; };
		function<int(int, int)> sum([](int a, int b) -> int {return a + b; });

		function<double(double, int)> mult(m);

		str a;
		function<void(int&)> inc(a);

		assert(max(3, 5) == 5);
		assert(max(12, 5 + 7) == 12);
		assert(max(0, 0) == 0);

		assert(sum(7, 5) == 12);
		assert(sum(13, -13) == 0);

		double sum_d_for_n = 0;
		double _d = 3.2;
		int _n = 5;
		for (size_t i = 0; i < _n; ++i)
			sum_d_for_n += _d;

		assert((sum_d_for_n - mult(double(_d), int(_n))) < 1e-13);

		int i, j;
		i = j = 5;
		inc(i);

		assert(i == (--j));
	}

	{
		//assignment
		function<int(int, int)> sum([](int a, int b) -> int {return a + b; });
		function<int(int, int)> func = sum;

		assert(func(18, 5) == sum(18, 5));
		assert(func(287, -13) == sum(287, -13));
		assert(func(0, 0) == sum(0, 0));

		func = ([](int a, int b) -> int {return a - b; });

		assert(func(321, 212) == 321 - 212);
		assert(func(-123, -13) == -123 + 13);
		assert(func(0, 0) == 0);

		function<bool(int, int)> isEqual([](int a, int b) -> bool {return a == b; });
		func = isEqual;
		assert(func(321, -321) == (321 == -321));
		assert(func(15, 15) == (15 == 15));
		assert(func(0, 1) == (0 == 1));
	}

	{
		//swap
		function<int(int, int)> max = [](int a, int b) -> int {return a > b ? a : b; };
		function<int(int, int)> min(min_func);

		assert(max(289, 256) == 289);
		assert(max(12, 98 - 14) == 84);
		assert(max(-169, -32) == -32);

		assert(min(289, 256) == 256);
		assert(min(12, 98 - 14) == 12);
		assert(min(-169, -32) == -169);

		swap(max, min);

		assert(min(289, 256) == 289);
		assert(min(12, 98 - 14) == 84);
		assert(min(-169, -32) == -32);

		assert(max(289, 256) == 256);
		assert(max(12, 98 - 14) == 12);
		assert(max(-169, -32) == -169);
	}

	{
		//empty (bool, operator==, operator!=)
		function<int()> func;

		assert(bool(func) == 0);

		assert(func == NULL);
		assert(func == std::nullptr_t());
		assert(NULL == func);
		assert(std::nullptr_t() == func);

		func = function<int(void)>(g);

		assert(bool(func) == 1);

		assert(func != NULL);
		assert(func != std::nullptr_t());
		assert(NULL != func);
		assert(std::nullptr_t() != func);
	}

	{
		//target
		assert(test(function<int()>(f)) == 'f');
		assert(test(function<int()>(g)) == 'g');
		assert(test(function<int()>(u)) == '0');

		function<char(function<int()>)> _test = test;
		assert(_test(f) == 'f');
		assert(_test(g) == 'g');
		assert(_test(u) == '0');
	}

	{
		//target_type
		function<int(int, int)> max = [](int a, int b) -> int {return a > b ? a : b; };
		function<int(int, int)> min = [](int a, int b) { return a < b ? a : b; };
		function<int()> func1;
		function<int()> func2 = []()->int {return 0; };

		assert((min.target_type() == max.target_type()) == false);
		assert((func1.target_type() == max.target_type()) == false);
		assert((func2.target_type() == min.target_type()) == false);
		assert((func1.target_type() == func2.target_type()) == false);

		function<int(int, int)> sum(sum_func);
		function<int(int, int)> dif(dif_func);
		function<int()> func3(g);
		function<int()> func4(u);

		assert((sum.target_type() == dif.target_type()) == true);
		assert((func1.target_type() == dif.target_type()) == false);
		assert((func2.target_type() == sum.target_type()) == false);
		assert((func3.target_type() == func4.target_type()) == true);
		assert((func2.target_type() == func4.target_type()) == false);
	}

	{
		const function<bool(int)> a = [](int x) -> bool {
			//std::cout << "a" << std::endl;
			return x > 10;
		};

		const function<int(float)> b = [](float y) -> int {
			//std::cout << "b" << std::endl;
			return int(y * y);
		};

		const function<float(bool)> c = [](bool z) -> float {
			//std::cout << "c" << std::endl;
			return z ? 3.1f : 3.34f;
		};

		{
			auto d = compose(a);
			assert(d(true) == 0);

			auto e = compose(a, b);
			assert(e(true) == 0);

			auto h = compose(a, b, c);
			assert(h(true) == 1);

			auto k = compose(a, b, c, a);
			assert(k(true) == 0);

			auto l = compose(a, b, c, a, b);
			assert(l(true) == 0);

			auto m = compose(a, b, c, a, b, c);
			assert(m(true) == 1);
		}
	}

	std::cout << "All test passed!" << std::endl;

	return 0;
}
