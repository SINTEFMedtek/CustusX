#ifndef SSCBOUNDINGBOX3D_H_
#define SSCBOUNDINGBOX3D_H_

#include "boost/array.hpp"
#include "sscVector3D.h"
#include <vector>

// --------------------------------------------------------
namespace ssc
{
//namespace utils
//{
// --------------------------------------------------------

/**
 * \addtogroup sscMath
 * @{
 */

/** \brief Representation of an integer bounding box in 3D.
 *  The data are stored as {xmin,xmax,ymin,ymax,zmin,zmax}, 
 *  in order to simplify communication with vtk.  
 */
class IntBoundingBox3D: public boost::array<int, 6>
{
public:
	IntBoundingBox3D();
	explicit IntBoundingBox3D(int x0, int x1, int y0, int y1, int z0 = 0, int z1 = 0);
	explicit IntBoundingBox3D(const Vector3D& a, const Vector3D& b);
	explicit IntBoundingBox3D(const Eigen::Vector3i& a, const Eigen::Vector3i& b);
	explicit IntBoundingBox3D(const double* data);
	explicit IntBoundingBox3D(const int* data);

	Eigen::Vector3i bottomLeft() const;
	Eigen::Vector3i topRight() const;
	Eigen::Vector3i center() const;
	Eigen::Vector3i range() const;
	Eigen::Vector3i corner(int x, int y, int z) const;
	bool contains(const Eigen::Vector3i& p) const;
};
// --------------------------------------------------------

std::ostream& operator<<(std::ostream& s, const IntBoundingBox3D& data);

/** \brief Representation of a floating-point bounding box in 3D.
 *  The data are stored as {xmin,xmax,ymin,ymax,zmin,zmax}, 
 *  in order to simplify communication with vtk.  
 */
class DoubleBoundingBox3D: public boost::array<double, 6>
{
public:
	DoubleBoundingBox3D();
	explicit DoubleBoundingBox3D(double x0, double x1, double y0, double y1, double z0 = 0.0, double z1 = 0.0);
	explicit DoubleBoundingBox3D(const Vector3D& a, const Vector3D& b);
	explicit DoubleBoundingBox3D(const double* data);
	explicit DoubleBoundingBox3D(const IntBoundingBox3D& bb);
	explicit DoubleBoundingBox3D(const int* data);
	static DoubleBoundingBox3D fromViewport(const double* data);
	static DoubleBoundingBox3D fromCloud(std::vector<Vector3D> cloud);
	static DoubleBoundingBox3D fromString(const QString& text); ///< construct a bb from a string containing 6 whitespace-separated numbers
	static DoubleBoundingBox3D zero() { return DoubleBoundingBox3D(0,0,0,0,0,0); }

	Vector3D bottomLeft() const;
	Vector3D topRight() const;
	Vector3D center() const;
	Vector3D range() const;
	Vector3D corner(int x, int y, int z) const;
	bool contains(const Vector3D& p) const;
	DoubleBoundingBox3D unionWith(const DoubleBoundingBox3D& other) const;
};
// --------------------------------------------------------

bool similar(const DoubleBoundingBox3D& a, const DoubleBoundingBox3D& b, double tol = 1.0E-6);

std::ostream& operator<<(std::ostream& s, const DoubleBoundingBox3D& data);

/**
 * @}
 */

// --------------------------------------------------------
//} // namespace utils
//using namespace utils;
} // namespace ssc
// --------------------------------------------------------

#endif /*SSCBOUNDINGBOX3D_H_*/
