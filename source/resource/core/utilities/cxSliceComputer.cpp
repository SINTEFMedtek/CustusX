/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors 
   may be used to endorse or promote products derived from this software 
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
=========================================================================*/

#include "cxSliceComputer.h"
#include "cxDefinitions.h"
#include <math.h>

namespace cx
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

/**Initialize the computer with reasonable defaults.
 */
SliceComputer::SliceComputer() :
	mClinicalApplication(mdNEUROLOGICAL),
	mOrientType(otORTHOGONAL),
	mPlaneType(ptAXIAL),
	mFollowType(ftFIXED_CENTER),
	mFixedCenter(Vector3D(0,0,0)),
	m_rMt(Transform3D::Identity()),
	mToolOffset(0.0),
	mUseGravity(false),
	mGravityDirection(Vector3D(0,0,-1)) ,
	mUseViewOffset(false),
	mViewportHeight(1),
	mViewOffset(0.5),
	mUseConstrainedViewOffset(false)
{
}

SliceComputer::~SliceComputer()
{
}

/**Group the typical plane definition uses together.
 */
void SliceComputer::initializeFromPlane(PLANE_TYPE plane, bool useGravity, const Vector3D& gravityDir, bool useViewOffset, double viewportHeight, double toolViewOffset, CLINICAL_VIEW application, bool useConstrainedViewOffset)
{
	setPlaneType(plane);
	mClinicalApplication = application;

	if (plane == ptSAGITTAL || plane == ptCORONAL || plane == ptAXIAL )
	{
		setOrientationType(otORTHOGONAL);
		setFollowType(ftFIXED_CENTER);
	}
    else if (plane == ptANYPLANE || plane==ptRADIALPLANE || plane==ptSIDEPLANE)
	{
		setOrientationType(otOBLIQUE);
		setFollowType(ftFOLLOW_TOOL);

		setGravity(useGravity, gravityDir);
		setToolViewOffset(useViewOffset, viewportHeight, toolViewOffset, useConstrainedViewOffset); // TODO finish this one
	}
    else if (plane==ptTOOLSIDEPLANE)
    {
        setOrientationType(otOBLIQUE); //skal være dette, som side. Eller skal det det? Ja, tror det. Må henge på toolen når du roterer rundt global z-akse.
        setFollowType(ftFIXED_CENTER); //skal være dette, men er ikke sikker på at det funker med oblique... Nei.

        //HALLO, HALLO! Alle plasser som kaller denne metoden må bruke bordets ned-vektor!
        //Satt i kallet til denne funksjonen.
        setGravity(useGravity, gravityDir);

        //NYNY test. Må ha dette for at planet skal henge på tool-spissen. Kanskje ikke nå lengre! Etter å ha sørget for at center er inne i view
        //setToolViewOffset(useViewOffset, viewportHeight, toolViewOffset, useConstrainedViewOffset); // TODO finish this one

        //setToolViewOffset(useViewOffset, viewportHeight, toolViewOffset, false); // TODO finish this one
//        useViewOffset 1
//         viewportHeight 92.5743
//         toolViewOffset 0.25
//         useConstrainedViewOffset 1

    }
}

void SliceComputer::setClinicalApplication(CLINICAL_VIEW application)
{
	mClinicalApplication = application;
}

ORIENTATION_TYPE SliceComputer::getOrientationType() const
{
	return mOrientType;
}

PLANE_TYPE SliceComputer::getPlaneType() const
{
	return mPlaneType;
}

FOLLOW_TYPE SliceComputer::getFollowType() const
{
    return mFollowType;
}

Transform3D SliceComputer::getToolPosition() const
{
	return m_rMt;
}

/**Set the position of the navigation tool, using the
 * standard definition of a tool transform (given in Tool).
 */
void SliceComputer::setToolPosition(const Transform3D& rMt) 
{ 
	m_rMt = rMt; 
}

/**Set planes to orient relative to the global space (ORTHOGONAL),
 * or relative to the tool (OBLIQUE).
 */
void SliceComputer::setOrientationType(ORIENTATION_TYPE val) 
{ 
	mOrientType = val; 
}

/**What plane to compute given the other settings.
 */
void SliceComputer::setPlaneType(PLANE_TYPE val) 
{ 
	mPlaneType = val; 
}

/**Set center to use as basis for center calculations
 * if FOLLOW_CENTER is set.
 */
void SliceComputer::setFixedCenter(const Vector3D& center) 
{ 
	mFixedCenter = center; 
}

/**Set the follow type.
 * FOLLOW_TOOL means place the center relative to the tool tip.
 * FIXED_CENTER means place the center relative to the FixedCenter.  
 */
void SliceComputer::setFollowType(FOLLOW_TYPE val) 
{ 
	mFollowType = val; 
}

/**Set the gravity direction. If used, the base planes will be oriented according 
 * to the gravity direction.
 */
void SliceComputer::setGravity(bool use, const Vector3D& dir) 
{ 
	mUseGravity = use; 
	mGravityDirection = dir; 
}

/**Set a virtual extension of the tool.
 */
void SliceComputer::setToolOffset(double val) 
{ 
	mToolOffset = val; 
}

/**Set the real tool center (without tooloffset) to a given distance from the
 * top of a viewport. This is handled by setting the plane center accordingly.
 * Overrides FollowType.
 */
void SliceComputer::setToolViewOffset(bool use, double viewportHeight, double viewOffset, bool useConstrainedViewOffset) 
{
	mUseViewOffset = use; 
	mViewportHeight = viewportHeight; 
	mViewOffset = viewOffset;
	mUseConstrainedViewOffset = useConstrainedViewOffset;
}

/**see setToolViewOffset()
 */
void SliceComputer::setToolViewportHeight(double viewportHeight)
{
	mViewportHeight = viewportHeight; 
}

/**Calculate a slice plane given the defined parameters.
 */
SlicePlane SliceComputer::getPlane()  const
{
	std::pair<Vector3D,Vector3D> basis = generateBasisVectors();
	SlicePlane plane;
	plane.i = basis.first;
	plane.j = basis.second;
	plane.c = Vector3D(0,0,mToolOffset);

	// transform position from tool to reference space
	plane.c = m_rMt.coord(plane.c);
	// transform orientation from tool to reference space for the oblique case only
	if (mOrientType==otOBLIQUE)
	{
		plane.i = m_rMt.vector(plane.i);
		plane.j = m_rMt.vector(plane.j);
	}

    if (mPlaneType == ptTOOLSIDEPLANE)
    {
        // orient planes so that gravity is down
     //   plane = orientToGravity(plane);
        //orient the plane, so that even if the tool has been rotated, the up vector and gravity vector will lie in this plane.
        plane = this->orientToGravityAroundToolZAxisAndAlongTheOperatingTable(plane);
    }

	// try to to this also for oblique views, IF the ftFIXED_CENTER is set.
	// use special acs centermod algo
	plane.c = generateFixedIJCenter(mFixedCenter, plane.c, plane.i, plane.j);

	// set center so that it is a fixed distance from viewport top
	plane = applyViewOffset(plane);

	return plane; 
}

/**Apply the view offset which is defined as follows:
 * 
 * Position the tool tip in the viewport such that it is
 * a given distance from the top. The distance is given by
 * viewport height times a ratio: the viewOffset.
 * 
 * For p=tooltip, H=viewportHeight, V=viewOffset:
 * q = p - H(1/2-V)*j
 * where we are interested in only the j-component:
 * c_new = c + j*(q-c)*j
 */
SlicePlane SliceComputer::applyViewOffset(const SlicePlane& base) const
{
	if (!mUseViewOffset)
	{
		return base;
	}

	double centerOffset = this->getViewOffsetAbsoluteFromCenter();

	SlicePlane retval = base;
	if (mUseConstrainedViewOffset)
	{
		Vector3D toolOffsetCenter = m_rMt.coord(Vector3D(0,0,mToolOffset));
		Vector3D newCenter = toolOffsetCenter + centerOffset * base.j;
		double toolOffsetDistance = dot(newCenter - base.c, base.j);

		Vector3D toolCenter = m_rMt.coord(Vector3D(0,0,0));
		newCenter = toolCenter - centerOffset * base.j;
		double toolDistance = dot(newCenter - base.c, base.j);
		double usedDistance = std::min(toolOffsetDistance, toolDistance);
		retval.c = base.c + usedDistance * base.j; // extract j-component of newCenter
	}
	else
	{
		Vector3D toolCenter = m_rMt.coord(Vector3D(0,0,mToolOffset));
		Vector3D newCenter = toolCenter - centerOffset * base.j;
		double distance = dot(newCenter - base.c, base.j);
		retval.c = base.c + distance * base.j; // extract j-component of newCenter
	}
	return retval;
}

double SliceComputer::getViewOffsetAbsoluteFromCenter() const
{
	if (mPlaneType==ptRADIALPLANE)
		return 0; // position in the center

	return mViewportHeight*(0.5-mViewOffset);
}

/**Generate the <i,j> vector pair spanning the basis plane
 * used. The basis plane is used for calculations before gravity
 * is added.  
 * Use along with generateBasisOffset() to get the true
 * plane.
 */
std::pair<Vector3D,Vector3D> SliceComputer::generateBasisVectors() const
{
	switch (mClinicalApplication)
	{
	case mdRADIOLOGICAL:
		return this->generateBasisVectorsRadiology();
	case mdNEUROLOGICAL:
	default:
		return this->generateBasisVectorsNeurology();
	}
}

/** definitions of planes for the neurology application
 *
 */
std::pair<Vector3D,Vector3D> SliceComputer::generateBasisVectorsNeurology() const
{
	switch (mPlaneType)
	{
	// use left-right ordering for axial/coronal
	case ptAXIAL:       return std::make_pair(Vector3D(-1, 0, 0), Vector3D( 0,-1, 0));
	case ptCORONAL:     return std::make_pair(Vector3D(-1, 0, 0), Vector3D( 0, 0, 1));
	case ptSAGITTAL:    return std::make_pair(Vector3D( 0, 1, 0), Vector3D( 0, 0, 1));

	// use planes corresponding to the cx Tool definitions
	case ptANYPLANE:    return std::make_pair(Vector3D( 0,-1, 0), Vector3D( 0, 0,-1));
	case ptSIDEPLANE:   return std::make_pair(Vector3D(-1, 0, 0), Vector3D( 0, 0,-1));
	case ptRADIALPLANE: return std::make_pair(Vector3D( 0,-1, 0), Vector3D(-1, 0, 0));
    case ptTOOLSIDEPLANE: return std::make_pair(Vector3D(-1, 0, 0), Vector3D( 0, 0,-1)); //SIDE
	default:
		throw std::exception();
	}
}

/** definitions of planes for the radiology application
 *
 */
std::pair<Vector3D,Vector3D> SliceComputer::generateBasisVectorsRadiology() const
{
	switch (mPlaneType)
	{
	// use right-left ordering for axial/coronal
	case ptAXIAL:       return std::make_pair(Vector3D( 1, 0, 0), Vector3D( 0,-1, 0));
	case ptCORONAL:     return std::make_pair(Vector3D( 1, 0, 0), Vector3D( 0, 0, 1));
	case ptSAGITTAL:    return std::make_pair(Vector3D( 0, 1, 0), Vector3D( 0, 0, 1));

	// use planes corresponding to the cx Tool definitions
	case ptANYPLANE:    return std::make_pair(Vector3D( 0,-1, 0), Vector3D( 0, 0,-1));
	case ptSIDEPLANE:   return std::make_pair(Vector3D(-1, 0, 0), Vector3D( 0, 0,-1));
	case ptRADIALPLANE: return std::make_pair(Vector3D( 0,-1, 0), Vector3D(-1, 0, 0));
    case ptTOOLSIDEPLANE: return std::make_pair(Vector3D(-1, 0, 0), Vector3D( 0, 0,-1)); //SIDE
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
 * When k and g points in the same direction, the calculations gets unstable. 
 * In this case we use the input i,j definition. There is a gradual shift between
 * these two definitions as we approach the singularity.
 */
SlicePlane SliceComputer::orientToGravity(const SlicePlane& base) const
{
	if (!mUseGravity)
	{
		return base;
	}

	SlicePlane retval = base;
	const Vector3D k = cross(base.i, base.j); // plane normal. Constant
	Vector3D up;
	up = -mGravityDirection; // normal case

	// weight of nongravity, 0=<w=<1, 1 means dont use gravity
	double w_n = dot(up, k); 
	w_n = w_n*w_n; // square to keep stability near normal use.

	Vector3D i_g = cross(up, k); //  |i_g| varies from 0 to 1 depending on 1-w_n
    //Seemed to me that i_n needed a change to be good in the singularity situation. //jone, 20160712
    //Vector3D i_n = base.i; // |i_n|==1
    Vector3D i_n = -base.j; // |i_n|==1


	// set i vector to a weighted mean of the two definitions
	// can also experiment with a tanh function or simply a linear interpolation
	//
	// Note: i_g is already small here if w_n is small, this will increase
	// the effect of i_n. Investigate.
	//
	retval.i = i_g*(1.0-w_n) + i_n*w_n; 
	retval.i = retval.i.normal(); // |i|==1 
	retval.j = cross(k, retval.i);

    return retval;
}

/**
 * @brief SliceComputer::orientToGravityAroundToolZAxisAndAlongTheOperatingTable
 * @param base
 * @return There are two steps to orient a plane fully to gravity. First one can rotate the plane around the tool Z axis. Secondly one can tilt the plane so that the side edges are
 * parallel to the gravity direction. This method combines these steps.
 *
 * We use the vector along the tool axis and find a vector, i_perpendicular, which is perpendicular to the tool and the up vector.
 * We use this and the up vector to find the tool vector's projection, i', down on the plane which is perpendicular to the up vector.
 * Since the plane should be oriented to gravity, we can use the up vector for j'.
 *
 * i_perpendicular = up x toolvector
 * i' = i_perpendicular x up
 * j' = up
 *
 * As the tool vector gets parallel to the up vector the orientation of the plane will be undefined.
 * Therefore we use the plane normal, k, as i_perpendicular in this case. This is done through a
 * weighting of i_perpendicular and k depending of the angle between the tool vector and up.
 *
 */
SlicePlane SliceComputer::orientToGravityAroundToolZAxisAndAlongTheOperatingTable(const SlicePlane &base) const
{
    if (!mUseGravity)
    {
        return base;
    }

    SlicePlane retval = base;
    Vector3D up;
    up = -mGravityDirection; // normal case
    /* ORIGORIG
    const Vector3D k_mark = cross(-up, base.i); // plane normal. Constant
    const Vector3D j_mark = cross(k_mark, base.i); // plane normal. Constant
    */

    //ORIG
//    retval.i = base.i;
//    retval.j = j_mark.normal();

//    retval.i = Vector3D(-1, 0, 0); //Dette er globalt, så det blir jo feil å hardkode!
//    retval.j = Vector3D(0, -1, 0);

//    retval.i = m_rMt.vector(Vector3D(0, 0, -1)); //Heller ikke bra. Da følger rotasjonen bare toolen.
//    retval.j = m_rMt.vector(Vector3D(0, -1, 0));

//    retval.i = m_rMt.inv().vector(Vector3D(0, 0, -1)); //Ble litt bedre, men fremdeles helt sprøtt.
//    retval.j = m_rMt.inv().vector(Vector3D(0, -1, 0));

    //Vector3D toolVector = m_rMt.coord(Vector3D(0,0,0)).normal(); //orig
    //Vector3D toolVector = m_rMt.inv().coord(Vector3D(retval.c)).normal();
    //Vector3D toolVector = m_rMt.vector(Vector3D(mFixedCenter)).normal();
    //Vector3D toolVector = retval.c.normal();

    Vector3D k = -cross(base.i, base.j);
    Vector3D toolVector = m_rMt.vector(Vector3D(0, 0, 1));
    Vector3D i_perpendicular = cross(up, toolVector).normal();
    double w_n = dot(up, toolVector);
    w_n = w_n*w_n; // square to keep stability near normal use.
    //retval.i = i_g*(1.0-w_n) + i_n*w_n;

    //Må ha noe her antakelig,
    i_perpendicular = i_perpendicular*(1.0-w_n) + k*w_n;

    Vector3D i_mark = cross(i_perpendicular, up).normal();
    //Vector3D j_mark = cross(i_mark, i_perpendicular).normal();
    Vector3D j_mark = up;

    //j_mark = j_mark*(1.0-w_n) + -toolVector*w_n;



    //std::cout << "Vectors: " << toolVector << std::endl << i_mm << std::endl << i_m << std::endl << j_m << std::endl;

    //m_rMt.coord(Vector3D(0,0,0)).normal();
//Vectors: -0.548367 -0.547154  0.632389
//-0.755514        -0 -0.655133
//-0.655133         0  0.755514
// 0 -1  0

//    m_rMt.inv().coord(Vector3D(0,0,0)).normal();
//Vectors:  0.582887 -0.620096 -0.525095
//0.669314        0 0.742979
// 0.742979         0 -0.669314
// 0 -1  0


    //m_rMt.vector(Vector3D(0,0,0)).normal();
    //bare 0

    //m_rMt.coord(Vector3D(retval.c)).normal();
    //ble tull. Riktig orientert, men følger ikke toolens akse!

    //m_rMt.coord(Vector3D(mFixedCenter)).normal();
    //også tull. Riktig orientert, men følger ikke toolens akse!

    //m_rMt.inv().vector(Vector3D(mFixedCenter)).normal();
    //samme her

    //m_rMt.vector(Vector3D(retval.c)).normal();
    //og her

    //plane.c og mFixedCenter
    //samme!

    retval.i = i_mark;
    retval.j = j_mark;

    return retval;
}


} // namespace cx
