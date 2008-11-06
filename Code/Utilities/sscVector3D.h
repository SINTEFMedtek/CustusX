#ifndef SSCVECTOR3D_H_
#define SSCVECTOR3D_H_

#include "boost/array.hpp"

// --------------------------------------------------------
namespace ssc
{
namespace utils
{
// --------------------------------------------------------

bool similar(double a, double b, double tol=1.0E-6);

/** Representation of a point or vector in 3D.
 *  Supports standard mathematical operators.
 */
class Vector3D : public boost::array<double,3>
{
public:
	Vector3D();
	Vector3D(double x, double y, double z);
	explicit Vector3D(const double* data);
	explicit Vector3D(const int* data);
	
	Vector3D& operator+=(const Vector3D& b);
	Vector3D& operator-=(const Vector3D& b);
	Vector3D& operator*=(double b);
	Vector3D& operator/=(double b);
	Vector3D operator-() const;
	
	Vector3D normal() const;
	double length() const;
};
// --------------------------------------------------------

Vector3D operator+(const Vector3D& a, const Vector3D& b);
Vector3D operator-(const Vector3D& a, const Vector3D& b);
Vector3D operator*(double a, const Vector3D& b);
Vector3D operator*(const Vector3D& a, double b);
Vector3D operator/(const Vector3D& a, double b);
Vector3D multiply_elems(const Vector3D& a, const Vector3D& b);
Vector3D divide_elems(const Vector3D& a, const Vector3D& b);
Vector3D cross(const Vector3D& a, const Vector3D& b);
double dot(const Vector3D& a, const Vector3D& b);
bool similar(const Vector3D& a, const Vector3D& b, double tol=1.0E-6);
std::ostream& operator<<(std::ostream& s, const Vector3D& data);

// --------------------------------------------------------
} // namespace utils
using namespace utils;
} // namespace ssc
// --------------------------------------------------------

#endif /*SSCVECTOR3D_H_*/
