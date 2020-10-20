#include "geometry.h"

#define eps 1e-14
#define PI 3.1415926535

double determinantOfMatrix(vector<vector<double>>& mat, int n, int rows, int cols);
void getCofactor(vector<vector<double>>& mat, vector<vector<double>>& temp, int p, int q, int n);

bool isEqual(double a, double b) {
	return abs(a - b) < eps;
}

Point::Point(double x, double y) :x(x), y(y) { };

bool Point::operator==(const Point& p) const {
	return (isEqual(x, p.x) && isEqual(y, p.y));
}
bool Point::operator!=(const Point& p) const {
	return (!isEqual(x, p.x) || !isEqual(y, p.y));
}

Line::Line(Point p1, Point p2) :k((p1.y - p2.y) / (p1.x - p2.x)), b(p1.y - p1.x * k) { }

Line::Line(double k, double b) : k(k), b(b) { }

bool Line::operator==(const Line& l) const {
	return (isEqual(k, l.k) && isEqual(b, l.b));
}

bool Line::operator!=(const Line& l) const {
	return (!isEqual(k, l.k) || !isEqual(b, l.b));
}

Polygon::Polygon(const std::vector<Point>& arr) : arr(arr) {}

double s(Point p1, Point p2) {
	return sqrt(pow(p1.x - p2.x, 2) + pow(p1.y - p2.y, 2));
}

double Polygon::perimeter() const {
	double P = 0;
	for (size_t i = 1; i < arr.size(); i++)
		P += s(arr[i], arr[i - 1]);
	P += s(arr[0], arr[arr.size() - 1]);
	return P;
}

double Polygon::area() const {
	double S = 0;
	Point aver = { (arr[0].x + arr[1].x) / 2, (arr[0].y + arr[1].y) / 2 };
	for (size_t i = 1; i < arr.size(); i++)
		S += Triangle(arr[i], arr[i - 1], aver).area();
	S += Triangle(arr[0], arr[arr.size() - 1], aver).area();
	return S;
}

bool Polygon::operator==(const Shape& another) const {
	if (arr.size() == ((Polygon&)another).arr.size()) {
		size_t i;
		for (i = 0; i < arr.size(); i++)
			if (arr[i] == ((Polygon&)another).arr[0])
				break;

		bool isEqual1 = true, isEqual2 = true;

		for (size_t j = 0; j < arr.size(); j++)
			if (arr[(i + j) / arr.size()] != ((Polygon&)another).arr[j])
				isEqual1 = false;

		for (size_t j = 0; j < arr.size(); j++)
			if (arr[(arr.size() -1 - i + j) / arr.size()] != ((Polygon&)another).arr[j])
				isEqual2 = false;

		if (isEqual1 || isEqual2)
			return true;
		return false;
	}
	return false;
}

bool Polygon::operator!=(const Shape& another) const {
	return !(*this == another);
}

//bool Triangle::operator==(const Shape& another) const {
//	return Polygon(*this) == (Polygon&) another;
//}

void Polygon::rotate(Point center, double angle) {
	for (size_t i = 0; i < arr.size(); i++) {
		arr[i].x *= cos(angle);
		arr[i].y *= sin(angle);
	}
}

void Polygon::reflex(Point center) {
	for (size_t i = 0; i < arr.size(); i++) {
		Line ln(center, arr[i]);
		arr[i].x += 2 * (center.x - arr[i].x);
		arr[i].y = ln.y(arr[i].x);
	}
}

//затруднение с пониманием на координатах
void Polygon::reflex(Line axis) {
	for (size_t i = 0; i < arr.size(); i++) {
		Line perpendicular(Point(0,0), Point(1, 1));//?
		Point proection(2, 2); 
		arr[i].x += 2 * (proection.x - arr[i].x);
		arr[i].y = perpendicular.y(arr[i].x);
	}
}

void Polygon::scale(Point center, double coefficient) {
	for (size_t i = 0; i < arr.size(); i++) {
		Line ln(center, arr[i]);
		arr[i] = Point(arr[i].x * coefficient, ln.y(arr[i].x * coefficient));
	}
}

size_t Polygon::verticesCount() {
	return arr.size();
}

std::vector<Point> Polygon::getVertices() {
	return arr;
}

Ellipse::Ellipse(Point f1, Point f2, double sum) :f1(f1), f2(f2), sum(sum) {}

double Ellipse::eccentricity() {
	double c = (f2.x - f1.x) / 2;
	return c / (sum / 2);
}

Point Ellipse::center() {
	return Point((f2.x + f1.x) / 2, (f2.y + f1.y) / 2);
}

std::pair<Point, Point> Ellipse::focuses() {
	return { f1, f2 };
}

double Ellipse::perimeter() const {
	double a = sum / 2;
	double b = s(f1, f2);
	return 2 * PI * sqrt((a * a + b * b) / 2);
}

double Ellipse::area() const {
	double a = sum / 2;
	double b = s(f1, f2);
	return PI * a * b;
}

bool Ellipse::operator==(const Shape& another) const {
	double a = sum / 2;
	double b = s(f1, f2);

	double a2 = ((Ellipse&)another).sum / 2;
	double b2 = s(((Ellipse&)another).f1, ((Ellipse&)another).f2);

	return (a == a2) && (b == b2);
}

bool Ellipse::operator!=(const Shape& another) const {
	return !(*this == ((Ellipse&)another));
}

void Ellipse::rotate(Point center, double angle) {
	f1.x *= cos(angle);
	f1.y *= sin(angle);

	f2.x *= cos(angle);
	f2.y *= sin(angle);
}

void Ellipse::reflex(Point center) {
	Line ln1(center, f1);
	Line ln2(center, f2);
	f1.x += 2 * (center.x - f1.x);
	f1.y = ln1.y(f1.x);
	f2.x += 2 * (center.x - f2.x);
	f2.y = ln1.y(f2.x);
}

//c отражением и поворотом не понимаю логику расчёта по координатам
void Ellipse::reflex(Line axis) {
	Line pr(Point(0, 0), Point(1, 1));
	Point perpendicular;
	//* перпендикуляр - как расстояние от точки до прямой -?
	f1.x += 2 * (perpendicular.x - f1.x);
	f1.y = pr.y(f1.x);
	f2.x += 2 * (perpendicular.x - f2.x);
	f2.y = pr.y(f2.x);
}

void Ellipse::scale(Point center, double coefficient) {
	Line ln(f1, f2);
	f1 = Point(f1.x * coefficient, ln.y(f1.x * coefficient));
	f2 = Point(f2.x * coefficient, ln.y(f2.x * coefficient));
	sum *= coefficient;
}

Circle::Circle(Point p, double r) :Ellipse(p, p, 2 * r) { }

double Circle::radius() {
	return sum / 2;
}

//не понимаю, зачем число:
Rectangle::Rectangle(Point p1, Point p2, int fictive) :Polygon(vector<Point>(4)) {
	arr[0] = p1;
	arr[1] = { p1.x, p2.y };
	arr[2] = p2;
	arr[3] = { p2.x, p1.y };
}

Rectangle::Rectangle(double d) :Polygon(vector<Point>(4)) {
	d = (d < 1) ? d : 1 / d;
	arr[0] = { 0,0 };
	arr[1] = { 0, d };
	arr[2] = { 1, d };
	arr[3] = { 1, 0 };
}

Point Rectangle::center() {
	return Point((arr[0].x + arr[2].x) / 2, (arr[0].y + arr[2].y) / 2);
}

std::pair<Line, Line>Rectangle::diagonals() {
	return { Line(arr[0], arr[2]), Line(arr[1], arr[3]) };
}

Square::Square(Point p1, Point p2) :Rectangle(p1, p2, 0) {};

Circle Square::circumscribedCircle() {
	return Circle(Point((arr[0].x + arr[2].x) / 2, (arr[0].y + arr[2].y) / 2), sqrt(2) * abs(arr[0].x - arr[1].x) / 2);
}

Circle Square::inscribedCircle() {
	return Circle(Point((arr[0].x + arr[2].x) / 2, (arr[0].y + arr[2].y) / 2), abs(arr[0].x - arr[1].x));
}

Triangle::Triangle(const Point p1, const Point p2, const Point p3) :Polygon(vector<Point>(3)) {
	arr[0] = p1;
	arr[1] = p2;
	arr[2] = p3;
}

Point Triangle::averagePerpendicular() {
	vector < vector<vector<double>>> temp(4, vector<vector<double>>(3, std::vector<double>(3)));

	for (size_t i = 0; i < 3; i++) {
		temp[0][i][2] = temp[1][i][2] = temp[2][i][2] = temp[3][i][2] = 1;
		temp[0][i][1] = temp[1][i][1] = temp[3][i][1] = arr[i].y;
		temp[1][i][0] = temp[2][i][0] = temp[3][i][0] = arr[i].x;
		temp[0][i][0] = temp[2][i][1] = pow(arr[i].x, 2) + pow(arr[i].y, 2);
	}

	vector<double> d;

	for (size_t i = 0; i < 3; i++)
		d[0] = determinantOfMatrix(temp[i], 3, 3, 3);

	return Point(d[0] / (2 * d[1]), d[2] / (2 * d[3]));
}

Circle Triangle::circumscribedCircle() {
	double a, b, c;
	a = sqrt(pow(arr[0].x - arr[1].x, 2) + pow(arr[0].y + arr[1].y, 2));
	b = sqrt(pow(arr[1].x - arr[2].x, 2) + pow(arr[1].y + arr[2].y, 2));
	c = sqrt(pow(arr[2].x - arr[0].x, 2) + pow(arr[2].y + arr[0].y, 2));
	double p = a + b + c;

	Point center = averagePerpendicular();

	return Circle(center, a * b * c / (4 * sqrt(p * (p - a) * (p - b) * (p - c) / p)));
}

// Function to get cofactor of mat[p][q] in temp[][]. n is current 
// dimension of mat[][] 
void getCofactor(vector<vector<double>>& mat, vector<vector<double>>& temp, int p, int q, int n)
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
double determinantOfMatrix(vector<vector<double>>& mat, int n, int rows, int cols)
{
	double D = 0; // Initialize result 

	//  Base case : if matrix contains single element 
	if (n == 1)
		return mat[0][0];

	vector<vector<double>> temp(rows, std::vector<double>(cols));
	// To store cofactors 

	int sign = 1;  // To store sign multiplier 

	 // Iterate for each element of first row 
	for (int f = 0; f < n; f++)
	{
		// Getting Cofactor of mat[0][f] 
		getCofactor(mat, temp, 0, f, n);
		D += sign * mat[0][f] * determinantOfMatrix(temp, n - 1, rows, cols);

		// terms are to be added with alternate sign 
		sign = -sign;
	}

	return D;
}

Circle Triangle::inscribedCircle() {
	double a, b, c;
	a = sqrt(pow(arr[0].x - arr[1].x, 2) + pow(arr[0].y + arr[1].y, 2));
	b = sqrt(pow(arr[1].x - arr[2].x, 2) + pow(arr[1].y + arr[2].y, 2));
	c = sqrt(pow(arr[2].x - arr[0].x, 2) + pow(arr[2].y + arr[0].y, 2));
	double p = a + b + c;

	return Circle(Point((a * arr[0].x + b * arr[1].x + c * arr[2].x) / p,
		(a * arr[0].y + b * arr[1].y + c * arr[2].y) / p),
		2 * sqrt(p * (p - a) * (p - b) * (p - c) / p));
}

//центр масс
Point Triangle::centroid() {
	return Point((arr[0].x + arr[1].x + arr[2].x) / 3, (arr[0].y + arr[1].y + arr[2].y) / 3);
}

//ортоцентр
Point Triangle::orthocenter() {
	vector < vector<vector<double>>> temp(4, vector<vector<double>>(3, std::vector<double>(3)));

	for (size_t i = 0; i < 3; i++) {
		temp[0][i][2] = temp[1][i][2] = temp[2][i][2] = temp[3][i][2] = 1;
		temp[0][i][0] = temp[1][i][1] = temp[3][i][1] = arr[i].y;
		temp[1][i][0] = temp[2][i][1] = temp[3][i][0] = arr[i].x;
		temp[0][i][1] = pow(arr[i].y, 2) + arr[(i + 1) % 3].x * arr[(i + 2) % 3].x;
		temp[2][i][0] = pow(arr[i].x, 2) + arr[(i + 1) % 3].y * arr[(i + 2) % 3].y;
	}

	vector<double> d;

	for (size_t i = 0; i < 3; i++)
		d[0] = determinantOfMatrix(temp[i], 3, 3, 3);

	return Point(d[0] / d[1], d[2] / d[3]);
}

// прямая Эйлера
Line Triangle::EulerLine() {
	return Line(orthocenter(), centroid());
}

//окружность Эйлера
Circle Triangle::ninePointsCircle() {
	Point ort = orthocenter();
	Point aver = averagePerpendicular();

	Point center((ort.x + aver.x) / 2, (ort.y + aver.y) / 2);
	double a, b, c;
	a = sqrt(pow(arr[0].x - arr[1].x, 2) + pow(arr[0].y + arr[1].y, 2));
	b = sqrt(pow(arr[1].x - arr[2].x, 2) + pow(arr[1].y + arr[2].y, 2));
	c = sqrt(pow(arr[2].x - arr[0].x, 2) + pow(arr[2].y + arr[0].y, 2));
	double p = a + b + c;

	return Circle(center, a * b * c / (4 * sqrt(p * (p - a) * (p - b) * (p - c) / p)));
}
