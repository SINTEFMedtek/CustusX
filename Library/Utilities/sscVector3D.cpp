#include "sscVector3D.h"
#include <vtkMath.h>
#include "sscUtilHelpers.h"

// --------------------------------------------------------
namespace ssc
{
namespace utils
{
// --------------------------------------------------------

bool similar(double a, double b, double tol)
{
	return fabs(b-a) < tol;
}

// quick and dirty way to access functionality from vtkMath:
static vtkMath* mathEngine = vtkMath::New();
// --------------------------------------------------------

// --------------------------------------------------------
// common functionality
void add_left(double* a, const double* b, unsigned n)
{
	for (unsigned i=0; i<n; ++i)
		a[i]+=b[i];
}
void sub_left(double* a, const double* b, unsigned n)
{
	for (unsigned i=0; i<n; ++i)
		a[i]-=b[i];
}
void mul_elements_left(double* a, const double* b, unsigned n)
{
	for (unsigned i=0; i<n; ++i)
		a[i]*=b[i];
}
void div_elements_left(double* a, const double* b, unsigned n)
{
	for (unsigned i=0; i<n; ++i)
		a[i]/=b[i];
}
void mul_left(double* a, double b, unsigned n)
{
	for (unsigned i=0; i<n; ++i)
		a[i]*=b;
}
void div_left(double* a, double b, unsigned n)
{
	for (unsigned i=0; i<n; ++i)
		a[i]/=b;
}

// --------------------------------------------------------
// Vector3D implementation
Vector3D::Vector3D()
{
}
Vector3D::Vector3D(double x, double y, double z)
{
	elems[0]=x;
	elems[1]=y;
	elems[2]=z;
}
Vector3D::Vector3D(const double* data)
{
	std::copy(data, data+size(), elems);
}
Vector3D::Vector3D(const int* data)
{
	std::copy(data, data+size(), elems);
}
// --------------------------------------------------------
Vector3D& Vector3D::operator+=(const Vector3D& b)
{	add_left(begin(), b.begin(), size() ); return *this;}
Vector3D& Vector3D::operator-=(const Vector3D& b)
{	sub_left(begin(), b.begin(), size() ); return *this;}
Vector3D& Vector3D::operator*=(double b)
{	mul_left(begin(), b, size() ); return *this;}
Vector3D& Vector3D::operator/=(double b)
{	div_left(begin(), b, size() ); return *this;}
Vector3D Vector3D::operator-() const
{
	Vector3D a(*this);
	a*= -1.0;
	return a;
}
// --------------------------------------------------------

/** return the vector with length 1.
 */
Vector3D Vector3D::normal() const
{
	return *this/length();
}

/** return cartesian length of vector.
 */
double Vector3D::length() const
{
	double tmp;
	for (const_iterator i=begin(); i!=end(); ++i)
	tmp += (*i) * (*i);
	return sqrt(tmp);
}
// --------------------------------------------------------

// --------------------------------------------------------
// Vector3D functions
Vector3D operator+(const Vector3D& a, const Vector3D& b)
{	Vector3D r(a); r+=b; return r;}
Vector3D operator-(const Vector3D& a, const Vector3D& b)
{	Vector3D r(a); r-=b; return r;}
Vector3D operator*(double a, const Vector3D& b)
{	Vector3D r(b); r*=a; return r;}
Vector3D operator*(const Vector3D& a, double b)
{	Vector3D r(a); r*=b; return r;}
Vector3D operator/(const Vector3D& a, double b)
{	Vector3D r(a); r/=b; return r;}
Vector3D multiply_elems(const Vector3D& a, const Vector3D& b)
{	Vector3D r(a); mul_elements_left(r.begin(), b.begin(), a.size()); return r;}
Vector3D divide_elems(const Vector3D& a, const Vector3D& b)
{	Vector3D r(a); div_elements_left(r.begin(), b.begin(), a.size()); return r;}
Vector3D cross(const Vector3D& a, const Vector3D& b)
{
	Vector3D retval;
	mathEngine->Cross(a.begin(), b.begin(), retval.begin());
	return retval;
}
double dot(const Vector3D& a, const Vector3D& b)
{
	return mathEngine->Dot(a.begin(), b.begin());
}
bool similar(const Vector3D& a, const Vector3D& b, double tol)
{
	return (b-a).length() < tol;
}
std::ostream& operator<<(std::ostream& s, const Vector3D& data)
{
	return stream_range(s, data.begin(), data.end());
}
// --------------------------------------------------------

// --------------------------------------------------------
} // namespace utils
} // namespace ssc
// --------------------------------------------------------


