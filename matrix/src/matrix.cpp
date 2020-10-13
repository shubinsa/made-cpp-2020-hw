#include "matrix.h"

using namespace task;

#pragma once

Proxy::Proxy(double* ptr, size_t cols) {
	Proxy::ptr = ptr;
	Proxy::cols = cols;
}

double& Proxy::operator[](size_t col) {
	if (col < 0 || col >= cols) {
		OutOfBoundsException error;
		throw error;
	}
	return ptr[col];
}

double Proxy::operator[](size_t col) const {
	if (col < 0 || col >= cols) {
		OutOfBoundsException error;
		throw error;
	}
	return ptr[col];
}

Matrix::Matrix() {
	arr = new double* [1];
	arr[0] = new double[1];
	rows = cols = 1;

	arr[0][0] = 0;
}

Matrix::Matrix(size_t rows, size_t cols) {
	Matrix::rows = rows;
	Matrix::cols = cols;
	arr = new double* [rows];
	for (size_t i = 0; i < rows; i++)
		arr[i] = new double[cols];

	for (size_t i = 0; i < rows; i++)
		for (size_t j = 0; j < cols; j++)
			arr[i][j] = 0;

	for (size_t i = 0; i < rows; i++)
			arr[i][i] = 1;

}

Matrix::Matrix(const Matrix& copy) {
	rows = copy.rows;
	cols = copy.cols;
	arr = new double* [rows];
	for (size_t i = 0; i < rows; i++)
		arr[i] = new double[cols];
}

Matrix& Matrix::operator=(const Matrix& a) {
	if (&a != this) {
		for (size_t i = 0; i < rows; i++)
			delete[] arr[i];
		delete[] arr;

		rows = a.rows;
		cols = a.cols;
		arr = new double* [rows];
		for (size_t i = 0; i < rows; i++)
			arr[i] = new double[cols];

		for (size_t i = 0; i < rows; i++)
			for (size_t j = 0; j < cols; j++)
				arr[i][j] = a.arr[i][j];
	}
	return *this;
}

double& Matrix::get(size_t row, size_t col) {
	if (row < 0 || row >= rows || col < 0 || col >= cols) {
		OutOfBoundsException error;
		throw error;
	}
	return arr[row][col];
}

const double& Matrix::get(size_t row, size_t col) const {
	if (row < 0 || row >= rows || col < 0 || col >= cols) {
		OutOfBoundsException error;
		throw error;
	}
	return arr[row][col];
}

void Matrix::set(size_t row, size_t col, const double& value) {
	if (row < 0 || row >= rows || col < 0 || col >= cols) {
		OutOfBoundsException error;
		throw error;
	}
	arr[row][col] = value;
}

void Matrix::resize(size_t new_rows, size_t new_cols) {
	double** arr2 = arr;
	int rows2 = rows, cols2 = cols;

	rows = new_rows;
	cols = new_cols;

	arr = new double* [rows];
	for (size_t i = 0; i < rows; i++)
		arr[i] = new double[cols];

	for (size_t i = 0; i < rows; i++)
		for (size_t j = 0; j < cols; j++)
			arr[i][j] = ((i < rows2&& j < cols2) ? arr2[i][j] : 0);

	for (size_t i = 0; i < rows2; i++)
		delete[] arr2[i];
	delete[] arr2;
}

Proxy Matrix::operator[](size_t row) {
	if (row < 0 || row >= rows) {
		OutOfBoundsException error;
		throw error;
	}
	return Proxy(arr[row], cols);
}

const Proxy Matrix::operator[](size_t row) const {
	if (row < 0 || row >= rows) {
		OutOfBoundsException error;
		throw error;
	}
	return Proxy(arr[row], cols);
}

Matrix& Matrix::operator+=(const Matrix& a) {
	if (rows != a.rows || cols != a.cols) {
		SizeMismatchException error;
		throw error;
	}

	for (size_t i = 0; i < rows; i++)
		for (size_t j = 0; j < cols; j++)
			arr[i][j] += a.arr[i][j];
	return *this;
}

Matrix& Matrix::operator-=(const Matrix& a) {
	if (rows != a.rows || cols != a.cols) {
		SizeMismatchException error;
		throw error;
	}

	for (size_t i = 0; i < rows; i++)
		for (size_t j = 0; j < cols; j++)
			arr[i][j] -= a.arr[i][j];
	return *this;
}

Matrix& Matrix::operator*=(const Matrix& a) {
	if (rows != a.rows || cols != a.cols) {
		SizeMismatchException error;
		throw error;
	}

	for (size_t i = 0; i < rows; i++)
		for (size_t j = 0; j < cols; j++)
			arr[i][j] *= a.arr[i][j];
	return *this;
}

Matrix& Matrix::operator*=(const double& number) {
	for (size_t i = 0; i < rows; i++)
		for (size_t j = 0; j < cols; j++)
			arr[i][j] *= number;
	return *this;
}

Matrix Matrix::operator+(const Matrix& a) const {
	if (rows != a.rows || cols != a.cols) {
		SizeMismatchException error;
		throw error;
	}

	Matrix res(*this);
	for (size_t i = 0; i < rows; i++)
		for (size_t j = 0; j < cols; j++)
			res.arr[i][j] += a.arr[i][j];
	return res;
}

Matrix Matrix::operator-(const Matrix& a) const {
	if (rows != a.rows || cols != a.cols) {
		SizeMismatchException error;
		throw error;
	}

	Matrix res(*this);
	for (size_t i = 0; i < rows; i++)
		for (size_t j = 0; j < cols; j++)
			res.arr[i][j] -= a.arr[i][j];
	return res;
}

Matrix Matrix::operator*(const Matrix& a) const {
	if (rows != a.rows || cols != a.cols) {
		SizeMismatchException error;
		throw error;
	}

	Matrix res(*this);
	for (size_t i = 0; i < rows; i++)
		for (size_t j = 0; j < cols; j++)
			res.arr[i][j] *= a.arr[i][j];
	return res;
}

Matrix Matrix::operator*(const double& a) const {
	Matrix res(*this);
	for (size_t i = 0; i < rows; i++)
		for (size_t j = 0; j < cols; j++)
			res.arr[i][j] *= a;
	return res;
}

Matrix Matrix::operator-() const {
	Matrix res(*this);
	for (size_t i = 0; i < rows; i++)
		for (size_t j = 0; j < cols; j++)
			res.arr[i][j] *= -1;
	return res;
}

Matrix Matrix::operator+() const {
	Matrix res(*this);
	return res;
}

double Matrix::det() const { //определитель
	if (rows != cols) {
		SizeMismatchException error;
		throw error;
	}

	double D = determinantOfMatrix(arr, rows);
	return D;
}

void Matrix::transpose() {
	double** arr2 = arr;
	int rows2 = rows, cols2 = cols;

	rows = cols2;
	cols = rows2;

	arr = new double* [rows];
	for (size_t i = 0; i < rows; i++)
		arr[i] = new double[cols];

	for (size_t i = 0; i < rows2; i++)
		for (size_t j = 0; j < cols2; j++)
			arr[j][i] = arr2[i][j];

	for (size_t i = 0; i < rows2; i++)
		delete[] arr2[i];
	delete[] arr2;
}

Matrix Matrix::transposed() const {
	Matrix res(cols, rows);
	for (size_t i = 0; i < rows; i++)
		for (size_t j = 0; j < cols; j++)
			res[j][i] = arr[i][j];

	return res;
}

double Matrix::trace() const {
	if (rows != cols) {
		SizeMismatchException error;
		throw error;
	}

	double sum = 0;

	for (size_t i = 0; i < rows; i++)
		sum += arr[i][i];

	return sum;
}

std::vector<double> Matrix::getRow(size_t row) {
	if (row < 0 || row >= rows) {
		OutOfBoundsException error;
		throw error;
	}
	std::vector<double> res;
	for (size_t j = 0; j < cols; j++)
		res.push_back(arr[row][j]);

	return res;
}

std::vector<double> Matrix::getColumn(size_t column) {
	if (column < 0 || column >= cols) {
		OutOfBoundsException error;
		throw error;
	}
	std::vector<double> res;
	for (size_t i = 0; i < rows; i++)
		res.push_back(arr[i][column]);

	return res;
}

bool Matrix::operator==(const Matrix& a) const {
	if (rows == a.rows && cols == a.cols) {
		for (size_t i = 0; i < rows; i++)
			for (size_t j = 0; j < cols; j++) {
				double dif = (arr[i][j] - arr[i][j]);
				if (dif > EPS || dif < -EPS)
					return false;
			}
	}
	else
		return false;
	return true;
}

bool Matrix::operator!=(const Matrix& a) const {
	if (rows == a.rows && cols == a.cols) {
		for (size_t i = 0; i < rows; i++)
			for (size_t j = 0; j < cols; j++) {
				double dif = (arr[i][j] - arr[i][j]);
				if (dif > EPS || dif < -EPS)
					return true;
			}
	}
	else
		return true;
	return false;
}

size_t Matrix::getRows() const {
	return rows;
}

size_t Matrix::getCols() const {
	return cols;
}

Matrix task::operator*(const double& a, const Matrix& b) {
	Matrix res(b);
	for (size_t i = 0; i < res.getRows(); i++)
		for (size_t j = 0; j < res.getCols(); j++)
			res.set(i, j, res.get(i, j) * a);
	return res;
}

std::ostream& task::operator<<(std::ostream& output, const Matrix& matrix) {
	for (size_t i = 0; i < matrix.getRows(); i++) {
		for (size_t j = 0; j < matrix.getCols(); j++)
			output << matrix[i][j] << ' ';
		output << '\n';
	}

	return output;
}

std::istream& task::operator>>(std::istream& input, Matrix& matrix) {
	int rows;
	int cols;
	input >> rows;
	input >> cols;

	Matrix res(rows, cols);

	for (size_t i = 0; i < rows; i++)
		for (size_t j = 0; j < cols; j++)
			input >> res[i][j];

	matrix = res;

	return input;
}

// Function to get cofactor of mat[p][q] in temp[][]. n is current 
// dimension of mat[][] 
void Matrix::getCofactor(double** mat, double** temp, int p, int q, int n) const
{
	int i = 0, j = 0;

	// Looping for each element of the matrix 
	for (int row = 0; row < n; row++)
	{
		for (int col = 0; col < n; col++)
		{
			//  Copying into temporary matrix only those element 
			//  which are not in given row and column 
			if (row != p && col != q)
			{
				temp[i][j++] = mat[row][col];

				// Row is filled, so increase row index and 
				// reset col index 
				if (j == n - 1)
				{
					j = 0;
					i++;
				}
			}
		}
	}
}

/* Recursive function for finding determinant of matrix.
   n is current dimension of mat[][]. */
double Matrix::determinantOfMatrix(double** mat, int n) const
{
	double D = 0; // Initialize result 

	//  Base case : if matrix contains single element 
	if (n == 1)
		return mat[0][0];

	double** temp;
	temp = new double* [rows];
	for (size_t i = 0; i < rows; i++)
		temp[i] = new double[rows];
	// To store cofactors 

	int sign = 1;  // To store sign multiplier 

	 // Iterate for each element of first row 
	for (int f = 0; f < n; f++)
	{
		// Getting Cofactor of mat[0][f] 
		getCofactor(mat, temp, 0, f, n);
		D += sign * mat[0][f] * determinantOfMatrix(temp, n - 1);

		// terms are to be added with alternate sign 
		sign = -sign;
	}

	return D;
}