#ifndef SSCSLICECOMPUTER_H_
#define SSCSLICECOMPUTER_H_

#include "sscVector3D.h"
#include "sscTransform3D.h"

namespace ssc
{

/**A 2D slice plane in 3D.
 * i,j are perpendicular.
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
 */
class SliceComputer
{
public:
	/** 
	 */
	enum ORIENTATION_TYPE
	{
		otOBLIQUE, ///< orient planes relative to the tool space 
		otORTHOGONAL ///< orient planes relative to the image/reference space.
	};
	enum PLANE_TYPE
	{
		ptSAGITTAL,
		ptCORONAL,
		ptAXIAL,
		ptANYPLANE,
		ptSIDEPLANE,
		ptRADIALPLANE 
	};
	enum FOLLOW_TYPE
	{
		ftFOLLOW_TOOL,
		ftFIXED_CENTER
	};
	
	// setgravity / usegravity for all 3 planes
	// create tests for anyplanes + gravity
	// mAnyPlaneCenterOffset - howto add ??
	
public:
	SliceComputer();
	~SliceComputer();
	
	void setToolPosition(const Transform3D& rMt) { m_rMt = rMt; }
	void setOrientationType(ORIENTATION_TYPE val) { mOrientType = val; }	
	void setPlaneType(PLANE_TYPE val) { mPlaneType = val; }
	void setFixedCenter(const Vector3D& center) { mFixedCenter = center; }
	void setFollowType(FOLLOW_TYPE val) { mFollowType = val; }
	void setGravity(bool use, const Vector3D& dir) { mUseGravity = use; mGravityDirection = dir; }
		
	SlicePlane getPlane() const;	
	
private:
	std::pair<Vector3D,Vector3D> generateBasisVectors() const;
	//Vector3D generateACSCenter(const Vector3D& center_d, const Vector3D& cross_d, const Vector3D& i, const Vector3D& j) const;
	Vector3D generateFixedIJCenter(const Vector3D& center_r, const Vector3D& cross_r, const Vector3D& i, const Vector3D& j) const;
	SlicePlane orientToGravity(const SlicePlane& base);

private:
	Transform3D m_rMt;
	ORIENTATION_TYPE mOrientType;
	PLANE_TYPE mPlaneType;
	FOLLOW_TYPE mFollowType;
	Vector3D mFixedCenter;	
	double mToolOffset;
	bool mUseGravity;
	Vector3D mGravityDirection; 
};

} // namespace ssc

#endif /*SSCSLICECOMPUTER_H_*/
