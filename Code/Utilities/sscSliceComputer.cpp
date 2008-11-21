#include "sscSliceComputer.h"

namespace ssc
{

SlicePlane::SlicePlane(const Vector3D& c_, const Vector3D& i_, const Vector3D& j_) : 
	c(c_), i(i_), j(j_) 
{
}

std::ostream& operator<<(std::ostream& s, const SlicePlane& val)
{
	s << "center   : " << val.c << std::endl;
	s << "i_vector : " << val.i << std::endl;
	s << "j_vector : " << val.j << std::endl;
	return s;
}

bool similar(const SlicePlane& a, const SlicePlane& b)
{
	return similar(a.c, b.c) && similar(a.i, b.i) && similar(a.j, b.j);
}


SliceComputer::SliceComputer() :
	mOrientType(otORTHOGONAL),
	mPlaneType(ptAXIAL),
	mFollowType(ftFIXED_CENTER),
	mFixedCenter(Vector3D(0,0,0)),
	mToolOffset(0.0),
	mUseGravity(false),
	mGravityDirection(Vector3D(0,0,-1)) 
{
}

SliceComputer::~SliceComputer()
{
}

SlicePlane SliceComputer::getPlane()  const
{
	std::pair<Vector3D,Vector3D> basis = generateBasisVectors();
	SlicePlane plane;
	plane.i = basis.first;
	plane.j = basis.second;	
	plane.c = Vector3D(0,0,mToolOffset);
	
//	// use special acs centermod algo
//	if (mOrientType==otORTHOGONAL)
//	{
//		plane.c = generateACSCenter(mFixedCenter, m_rMt.coord(Vector3D(0,0,0)), plane.i, plane.j);
//	}
	
	plane.c = m_rMt.coord(plane.c);
	// transform from tool to reference space
	if (mOrientType==otOBLIQUE)
	{
		//plane.c = Vector3D(0,0,mOffset+mAnyPlaneCenterOffset);
		plane.i = m_rMt.vector(plane.i);
		plane.j = m_rMt.vector(plane.j);		
	}

	// use special acs centermod algo
	if (mOrientType==otORTHOGONAL) // is this algo applicable for oblique views?
	{
		plane.c = generateFixedIJCenter(mFixedCenter, plane.c, plane.i, plane.j);
	}
		
	return plane; 
}

//ViewData LayoutGenerator::generateAnyplaneViewData(const LayerData& layer) const
//{
//	// define plane data in tool place:
//	Vector3D center(0,0,mOffset+mAnyPlaneCenterOffset);
//	Vector3D i(0,-1,0);
//	Vector3D j(0,0,-1);
//	// convert to reference space
//	center = getToolPos(layer).coord(center);
//	i = getToolPos(layer).vector(i);
//	j = getToolPos(layer).vector(j);
//
//	ViewData view(vaANYPLANE,	center, i, j);
//	// gravity-orient if applicable.
//	if (mUseGForce)
//		view = orientToGravity(view);
//	return view;
//}

std::pair<Vector3D,Vector3D> SliceComputer::generateBasisVectors() const
{
	switch (mPlaneType)
	{
	case ptAXIAL:       return std::make_pair(Vector3D(-1, 0, 0), Vector3D( 0,-1, 0)); 
	case ptCORONAL:     return std::make_pair(Vector3D(-1, 0, 0), Vector3D( 0, 0, 1)); 
	case ptSAGITTAL:    return std::make_pair(Vector3D( 0, 1, 0), Vector3D( 0, 0, 1)); 
	case ptANYPLANE:    return std::make_pair(Vector3D( 0,-1, 0), Vector3D( 0, 0,-1)); 
	case ptSIDEPLANE:   return std::make_pair(Vector3D(-1, 0, 0), Vector3D( 0, 0,-1)); 
	case ptRADIALPLANE: return std::make_pair(Vector3D( 0,-1, 0), Vector3D(-1, 0, 0));
	default:
		throw std::exception();
	}
}

/**Generate a viewdata containing a slice that always keeps the center 
 * of the observed image at center, but the z-component varies according 
 * to cross.
 * Input is the i,j vectors defining the slice, and center,cross positions,
 * all in ref space. 
 * 
 */
Vector3D SliceComputer::generateFixedIJCenter(const Vector3D& center_r, const Vector3D& cross_r, const Vector3D& i, const Vector3D& j) const
{
	if (mFollowType==ftFIXED_CENTER)
	{	
		// r is REF, s is SLICE
		Transform3D M_rs = createTransformIJC(i, j, Vector3D(0,0,0)); // transform from data to slice, zero center.
		Transform3D M_sr = M_rs.inv();
		Vector3D center_s = M_sr.coord(center_r);
		Vector3D cross_s = M_sr.coord(cross_r);
		// in SLICE space, use {xy} values from center and {z} value from cross.
		Vector3D q_s(center_s[0], center_s[1], cross_s[2]);
		Vector3D q_r = M_rs.coord(q_s);
		return q_r;
	}
	return cross_r;
}

/**Modify the i,j vector such that j points as much as possible towards 
 * the gravity up direction, while not changing the plane itself (i.e. k is constant).
 * 
 * This is done by computing 
 *   i' = -g x k
 *   j' =  k x i'
 * 
 * If the input j vector point upwards, we flip gravity so that the image dont flip.
 * 
 * When k and g points in the same direction, the calculations gets unstable. 
 * In this case we use the input i,j definition. There is a gradual shift between
 * these two definitions as we approach the singularity.
 */
SlicePlane SliceComputer::orientToGravity(const SlicePlane& base)
{
	if (!mUseGravity)
	{
		return base;
	}
	
	SlicePlane retval = base;	
	const Vector3D k = cross(base.i, base.j); // plane normal. Constant
	Vector3D up;
	
	// find the up direction. This removes flips.
	if (dot(base.j, mGravityDirection) > 0.0)
	{
		up = -mGravityDirection;
	}
	else
	{
		up = mGravityDirection;		
	}
	
	// weight of nongravity, 0=<w=<1, 1 means dont use gravity
	double w_n = dot(up, k); 
	w_n = w_n*w_n; // square to keep stability near normal use.
	
	Vector3D i_g = cross(up, k); //  |i_g| varies from 0 to 1 depending on 1-w_n
	Vector3D i_n = base.i; // |i_n|==1
	
	// set i vector to a weighted mean of the two definitions
	// can also experiment with a tanh function or simply a linear interpolation
	//
	// Note: i_g is already small here if w_n is small, this will increase
	// the effect of i_n. Investigate.
	//
	retval.i = i_g*(1.0-w_n) + i_n*w_n; 
	retval.i = retval.i.normal(); // |i|==1 
	retval.j = cross(k, retval.i);
}

/**Orientate a view to gravity as follows:
 * The plane k-vector is unchanged.
 * The plane new j-vector shall point upwards as much as possible,
 * achieve this by finding the new i-vector as orthogonal to up:
 *   i = up x k
 * 	 j = k x i
 */
//ViewData SliceComputer::orientToGravity(const ViewData& input) const
//{
//	ViewData view = input;
//	Vector3D g_up = -mGForce;
//	Vector3D k = cross(input.IVector, input.JVector);
//	
//	Vector3D i_new = cross(g_up, k);
//	if (i_new.length()<10.0E-4)
//		i_new = input.IVector;
//	else
//		i_new = i_new.normal(); 
//	
//	Vector3D j_new = cross(k, i_new);
//	
//	view.IVector = i_new;
//	view.JVector = j_new;
//	return view;	
//}

///**Generate a viewdata containing a slice that always keeps the center 
// * of the observed image at center, but the z-component varies according 
// * to cross.
// * Input is the i,j vectors defining the slice, and center,cross positions,
// * all in DATA space. 
// * 
// */
//Vector3D SliceComputer::generateACSCenter(const Vector3D& center_r, const Vector3D& cross_r, const Vector3D& i, const Vector3D& j) const
//{
//	if (mFollowType==ftFIXED_CENTER)
//	{	
//		// r is REF, s is SLICE
//		Transform3D M_rs = createTransformIJC(i, j, Vector3D(0,0,0)); // transform from data to slice, zero center.
//		Transform3D M_sr = M_rs.inv();
//		Vector3D center_s = M_sr.coord(center_r);
//		Vector3D cross_s = M_sr.coord(cross_r);
//		// in SLICE space, use {xy} values from center and {z} value from cross.
//		Vector3D q_s(center_s[0], center_s[1], cross_s[2]);
//		Vector3D q_r = M_rs.coord(q_s);
//		return q_r;
//	}
//	else // ftFOLLOW_TOOL
//	{
//		return m_rMt.coord(Vector3D(0,0,0));
//	}
//}





} // namespace ssc
