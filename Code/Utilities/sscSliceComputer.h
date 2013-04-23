#ifndef SSCSLICECOMPUTER_H_
#define SSCSLICECOMPUTER_H_

#include "sscTransform3D.h"
#include "sscDefinitions.h"

namespace ssc
{

/**\brief A 2D slice plane in 3D.
 * i,j are perpendicular unit vectors.
 *
 * \ingroup sscUtility
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


/**\brief Calculates a slice plane given a definition.
 * 
 * SliceComputer is set up with what image to slice and the tool to use,
 * along with what method that defines the slicing. The input produces a 
 * slice plane defined by the i,j basis vectors spanning the plane, and a 
 * center c defining the center. 
 * 
 * Most methods set the computer state, while getPlane() does all the computing.
 *
 * \ingroup sscUtility
 */
class SliceComputer
{
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
	void setToolViewOffset(bool use, double viewportHeight, double viewOffset, bool useConstrainedViewOffset = false);
	void setToolViewportHeight(double viewportHeight);
	void setClinicalApplication(CLINICAL_APPLICATION application);
	SlicePlane getPlane() const;

	void initializeFromPlane(PLANE_TYPE plane, bool useGravity, const Vector3D& gravityDir, bool useViewOffset, double viewportHeight, double toolViewOffset, CLINICAL_APPLICATION application, bool useConstrainedViewOffset = false);
	void switchOrientationMode(ORIENTATION_TYPE type);
	ORIENTATION_TYPE getOrientationType() const;
	PLANE_TYPE getPlaneType() const;
	Transform3D getToolPosition() const;

private:
	std::pair<Vector3D,Vector3D> generateBasisVectors() const;
	Vector3D generateFixedIJCenter(const Vector3D& center_r, const Vector3D& cross_r, const Vector3D& i, const Vector3D& j) const;
	SlicePlane orientToGravity(const SlicePlane& base) const;
	SlicePlane applyViewOffset(const SlicePlane& base) const;

	std::pair<Vector3D,Vector3D> generateBasisVectorsNeurology() const;
	std::pair<Vector3D,Vector3D> generateBasisVectorsRadiology() const;

private:
	CLINICAL_APPLICATION mClinicalApplication;
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
	bool mUseConstrainedViewOffset;
};

} // namespace ssc

#endif /*SSCSLICECOMPUTER_H_*/
