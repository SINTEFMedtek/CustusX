/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXBOUNDINGBOX3D_H_
#define CXBOUNDINGBOX3D_H_

#include "cxResourceExport.h"
#include "cxPrecompiledHeader.h"

#include "boost/array.hpp"
#include "cxVector3D.h"
#include <vector>

// --------------------------------------------------------
namespace cx
{
//namespace utils
//{
// --------------------------------------------------------

/**
 * \addtogroup cx_resource_core_math
 * @{
 */

/** \brief Representation of an integer bounding box in 3D.
 *  The data are stored as {xmin,xmax,ymin,ymax,zmin,zmax}, 
 *  in order to simplify communication with vtk.  
 */
class cxResource_EXPORT IntBoundingBox3D: public boost::array<int, 6>
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

cxResource_EXPORT std::ostream& operator<<(std::ostream& s, const IntBoundingBox3D& data);

/** \brief Representation of a floating-point bounding box in 3D.
 *  The data are stored as {xmin,xmax,ymin,ymax,zmin,zmax}, 
 *  in order to simplify communication with vtk.  
 */
class cxResource_EXPORT DoubleBoundingBox3D: public boost::array<double, 6>
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

cxResource_EXPORT bool similar(const DoubleBoundingBox3D& a, const DoubleBoundingBox3D& b, double tol = 1.0E-6);

cxResource_EXPORT std::ostream& operator<<(std::ostream& s, const DoubleBoundingBox3D& data);

/**
 * Create the bounding box that is contained in both a and b.
 * No intersection gives a zero box.
 */
cxResource_EXPORT DoubleBoundingBox3D intersection(DoubleBoundingBox3D a, DoubleBoundingBox3D b);

/**
 * @}
 */

// --------------------------------------------------------
//} // namespace utils
//using namespace utils;
} // namespace cx
// --------------------------------------------------------

#endif /*CXBOUNDINGBOX3D_H_*/
