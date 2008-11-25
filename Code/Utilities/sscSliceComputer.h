#ifndef SSCSLICECOMPUTER_H_
#define SSCSLICECOMPUTER_H_

#include "sscVector3D.h"
#include "sscTransform3D.h"

namespace ssc
{

/**A 2D slice plane in 3D.
 * i,j are perpendicular unit vectors.
 */
class SlicePlane
{
public:
	SlicePlane() {}
	SlicePlane(const Vector3D& c, const Vector3D& i, const Vector3D& j);
	Vector3D c; ///< defines the center of the plane
	Vector3D i; ///< defines the first axis of the plane. unit vector
	Vector3D j; ///< defines the second axis of the plane. unit vector
};

std::ostream& operator<<(std::ostream& s, const SlicePlane& val);
bool similar(const SlicePlane& a, const SlicePlane& b);

/**Calculates a slice plane given a definition.
 * 
 * SliceComputer is set up with what image to slice and the tool to use,
 * along with what method that defines the slicing. The input produces a 
 * slice plane defined by the i,j basis vectors spanning the plane, and a 
 * center c defining the center. 
 * 
 * Most methods set the computer state, while getPlane() does all the computing.
 */
class SliceComputer
{
public:
	enum ORIENTATION_TYPE
	{
		otOBLIQUE,   ///< orient planes relative to the tool space 
		otORTHOGONAL ///< orient planes relative to the image/reference space.
	};
	enum PLANE_TYPE
	{
		ptSAGITTAL,   ///< a slice seen from the side of the patient
		ptCORONAL,    ///< a slice seen from the front of the patient
		ptAXIAL,      ///< a slice seen from the top of the patient
		ptANYPLANE,   ///< a plane aligned with the tool base plane
		ptSIDEPLANE,  ///< z-rotated 90* relative to anyplane (dual anyplane)
		ptRADIALPLANE ///< y-rotated 90* relative to anyplane (bird's view)
	};
	enum FOLLOW_TYPE
	{
		ftFOLLOW_TOOL, ///< center follows tool
		ftFIXED_CENTER ///< center is set.
	};
	
public:
	SliceComputer();
	~SliceComputer();
	void setToolPosition(const Transform3D& rMt);
	void setOrientationType(ORIENTATION_TYPE val);	
	void setPlaneType(PLANE_TYPE val);
	void setFixedCenter(const Vector3D& center); 
	void setFollowType(FOLLOW_TYPE val); 
	void setGravity(bool use, const Vector3D& dir);
	void setToolOffset(double val); 
	void setToolViewOffset(bool use, double viewportHeight, double viewOffset);
	SlicePlane getPlane() const;	
	
private:
	std::pair<Vector3D,Vector3D> generateBasisVectors() const;
	Vector3D generateFixedIJCenter(const Vector3D& center_r, const Vector3D& cross_r, const Vector3D& i, const Vector3D& j) const;
	SlicePlane orientToGravity(const SlicePlane& base) const;
	Transform3D generateBasisOffset() const;
	SlicePlane applyViewOffset(const SlicePlane& base) const;

private:
	ORIENTATION_TYPE mOrientType;
	PLANE_TYPE mPlaneType;
	FOLLOW_TYPE mFollowType;
	Vector3D mFixedCenter;	

	Transform3D m_rMt;
	double mToolOffset;
	
	bool mUseGravity;
	Vector3D mGravityDirection; 

	bool mUseViewOffset;
	double mViewportHeight;
	double mViewOffset;
};

} // namespace ssc

#endif /*SSCSLICECOMPUTER_H_*/
