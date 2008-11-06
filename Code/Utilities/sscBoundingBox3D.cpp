#include "sscBoundingBox3D.h"
#include "sscUtilHelpers.h"

// --------------------------------------------------------
namespace ssc
{
namespace utils
{
// --------------------------------------------------------


// --------------------------------------------------------
// IntBoundingBox3D
IntBoundingBox3D::IntBoundingBox3D()
{}
IntBoundingBox3D::IntBoundingBox3D(int x0, int x1, int y0, int y1, int z0, int z1)
{
	elems[0] = x0; elems[1] = x1;
	elems[2] = y0; elems[3] = y1;
	elems[4] = z0; elems[5] = z1;
}
/**Create a bounding box describing the volume spanned by the two points a and b.
 */
IntBoundingBox3D::IntBoundingBox3D(const Vector3D& a, const Vector3D& b)
{
	Vector3D bl, tr;
	for (unsigned i=0; i<a.size(); ++i)
	{
		bl[i] = std::min(a[i], b[i]);
		tr[i] = std::max(a[i], b[i]);
	}

	elems[0] = static_cast<int>(bl[0]); elems[1] = static_cast<int>(tr[0]);
	elems[2] = static_cast<int>(bl[1]); elems[3] = static_cast<int>(tr[1]);
	elems[4] = static_cast<int>(bl[2]); elems[5] = static_cast<int>(tr[2]);
}
IntBoundingBox3D::IntBoundingBox3D(const double* data)
{
	for (unsigned i=0; i<size(); ++i)
	elems[i] = static_cast<int>(data[i]);
	//	std::copy(data, data+size(), elems); 
}
IntBoundingBox3D::IntBoundingBox3D(const int* data)
{	std::copy(data, data+size(), elems);}
// --------------------------------------------------------
Vector3D IntBoundingBox3D::bottomLeft() const
{	return Vector3D(elems[0], elems[2], elems[4]);}
Vector3D IntBoundingBox3D::topRight() const
{	return Vector3D(elems[1], elems[3], elems[5]);}
Vector3D IntBoundingBox3D::center() const
{	return (bottomLeft()+topRight())/2.0;}
Vector3D IntBoundingBox3D::range() const
{	return topRight()-bottomLeft();}
/**Return the coordinate value of a given corner.
 * Insert 0 or 1 for each of the 3 axis in order to get start or 
 * stop corner along that axis.  
 */
Vector3D IntBoundingBox3D::corner(int x, int y, int z) const
{
	Vector3D c;
	c[0] = x ? elems[0] : elems[1];
	c[1] = y ? elems[0] : elems[1];
	c[2] = z ? elems[0] : elems[1];
	return c;
}
// --------------------------------------------------------
std::ostream& operator<<(std::ostream& s, const IntBoundingBox3D& data)
{
	return stream_range(s, data.begin(), data.end());
}
// --------------------------------------------------------

// --------------------------------------------------------
// DoubleBoundingBox3D 
DoubleBoundingBox3D::DoubleBoundingBox3D()
{}
DoubleBoundingBox3D::DoubleBoundingBox3D(double x0, double x1, double y0, double y1, double z0, double z1)
{
	elems[0] = x0; elems[1] = x1;
	elems[2] = y0; elems[3] = y1;
	elems[4] = z0; elems[5] = z1;
}
/**Create a bounding box describing the volume spanned by the two points a and b.
 */
DoubleBoundingBox3D::DoubleBoundingBox3D(const Vector3D& a, const Vector3D& b)
{
	Vector3D bl, tr;
	for (unsigned i=0; i<a.size(); ++i)
	{
		bl[i] = std::min(a[i], b[i]);
		tr[i] = std::max(a[i], b[i]);
	}

	elems[0] = bl[0]; elems[1] = tr[0];
	elems[2] = bl[1]; elems[3] = tr[1];
	elems[4] = bl[2]; elems[5] = tr[2];
}
DoubleBoundingBox3D::DoubleBoundingBox3D(const double* data)
{	std::copy(data, data+size(), elems);}
DoubleBoundingBox3D::DoubleBoundingBox3D(const int* data)
{	std::copy(data, data+size(), elems);}
DoubleBoundingBox3D::DoubleBoundingBox3D(const IntBoundingBox3D& bb)
{
	for (unsigned i=0; i<size(); ++i)
	{
		elems[i] = bb.elems[i];
	}
}
// --------------------------------------------------------

/**Create a bounding box based the following input: (xmin,ymin,xmax,ymax)
 */
DoubleBoundingBox3D DoubleBoundingBox3D::fromViewport(const double* data)
{
	return DoubleBoundingBox3D(data[0], data[2], data[1], data[3], 0, 0);
}
// --------------------------------------------------------

Vector3D DoubleBoundingBox3D::bottomLeft() const
{	return Vector3D(elems[0], elems[2], elems[4]);}
Vector3D DoubleBoundingBox3D::topRight() const
{	return Vector3D(elems[1], elems[3], elems[5]);}
Vector3D DoubleBoundingBox3D::center() const
{	return (bottomLeft()+topRight())/2.0;}
Vector3D DoubleBoundingBox3D::range() const
{	return topRight()-bottomLeft();}
/**Return the coordinate value of a given corner.
 * Insert 0 or 1 for each of the 3 axis in order to get start or 
 * stop corner along that axis.  
 */
Vector3D DoubleBoundingBox3D::corner(int x, int y, int z) const
{
	Vector3D c;
	c[0] = x ? elems[0] : elems[1];
	c[1] = y ? elems[0] : elems[1];
	c[2] = z ? elems[0] : elems[1];
	return c;
}
// --------------------------------------------------------

/** return true if p is inside or on the edge of the bounding box
 */
bool DoubleBoundingBox3D::contains(const Vector3D& p) const
{
	bool inside = true;
	for (unsigned i=0; i<3; ++i)
		inside &= ((elems[2*i] <= p[i]) && (p[i] <= elems[2*i+1]));
	return inside;
}


bool similar(const DoubleBoundingBox3D& a, const DoubleBoundingBox3D& b, double tol)
{
	return similar(a.bottomLeft(), b.bottomLeft(), tol) && similar(a.topRight(), b.topRight(), tol);
}
// --------------------------------------------------------
std::ostream& operator<<(std::ostream& s, const DoubleBoundingBox3D& data)
{
	return stream_range(s, data.begin(), data.end());
}
// --------------------------------------------------------

// --------------------------------------------------------
} // namespace utils
} // namespace ssc
// --------------------------------------------------------
