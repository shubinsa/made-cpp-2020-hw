#pragma once
#include <vector>
#include <iostream>
#include <cstdint>
#include <cmath>

namespace task {

	using std::cin;
	using std::cout;
	using std::endl;
	using std::vector;

	vector<double> scan_vec() {
		size_t N;
		cin >> N;

		vector<double> arr;

		for (size_t i = 0; i < N; ++i) {
			double x;
			cin >> x;
			arr.push_back(x);
		}

		return arr;
	}

	void print_vec(vector<double> x) {
		for (size_t i = 0; i < x.size(); ++i) {
			cout << x[i] << " ";
		}
		cout << endl;
	}
	vector<double> operator+ (
		const vector<double>& a,
		const vector<double>& b) {

		vector<double> c;

		for (size_t i = 0; i < a.size(); ++i) {
			c.push_back(a[i] + b[i]);
		}
		return c;
	}

	vector<double> operator+ (const vector<double>& a) {
		return a;
	}

	vector<double> operator- (
		const vector<double>& a,
		const vector<double>& b) {

		vector<double> c;

		for (size_t i = 0; i < a.size(); ++i) {
			c.push_back(a[i] - b[i]);
		}
		return c;
	}

	vector<double> operator- (const vector<double>& a) {
		vector<double> c;

		for (size_t i = 0; i < a.size(); ++i) {
			c.push_back(-a[i]);
		}
		return c;
	}


	double operator* (
		const vector<double>& a,
		const vector<double>& b) {

		double c = 0;

		for (size_t i = 0; i < a.size(); ++i) {
			c += a[i] * b[i];
		}

		return c;
	}

	vector<double> operator% (
		const vector<double>& a,
		const vector<double>& b) {

		vector<double> c;

		int a_x_b_y;
		int a_y_b_x;

		a_x_b_y = a[0] * b[1];
		a_y_b_x = a[1] * b[0];

		c[0] = 0;
		c[1] = 0;
		c[3] = abs(a_x_b_y - a_y_b_x);

		return c;
	}

	bool operator|| (
		const vector<double>& a,
		const vector<double>& b) {

		vector<double> c;
		for (size_t i = 0; i < a.size() - 1; ++i) {
			if (a[i] / b[i] != a[i + 1] / b[i + 1]) {
				return false;
			}

		}

		return true;
	}

	bool operator&& (
		const vector<double>& a,
		const vector<double>& b) {

		if ((a[0] / b[0]) < 0)
			return false;
		for (size_t i = 0; i < a.size() - 1; ++i) {
			if (a[i] / b[i] != a[i + 1] / b[i + 1]) {
				return false;
			}

		}

		return true;
	}

	std::istream& operator>>(std::istream& is, vector<double>& a) {
		int N;
		is >> N;

		a.clear();

		for (int i = 0; i < N; i++) {
			int  k;
			is >> k;
			a.push_back(k);
		}

		return is;
	}

	std::ostream& operator<<(std::ostream& os, const vector<double>& a) {
		int N = a.size();

		for (int i = 0; i < N; i++) {
			os << a[i] << " ";
		}
		os << endl;
		return os;
	}

	vector<double> reverse(vector<double> a) {
		vector<double> b;
		for (size_t i = a.size() - 1; i < -1; --i) {
			b.push_back(a[i]);
		}
		return b;
	}

	vector<int> operator| (
		const vector<int>& a,
		const vector<int>& b) {

		vector<int> c;

		for (size_t i = 0; i < a.size(); ++i) {
			c.push_back(a[i] | b[i]);
		}
		return c;
	}

	vector<int> operator& (
		const vector<int>& a,
		const vector<int>& b) {

		vector<int> c;

		for (size_t i = 0; i < a.size(); ++i) {
			c.push_back(a[i] & b[i]);
		}
		return c;
	}

}
