#include "sscFrame3D.h"
#include <math.h>

// --------------------------------------------------------
namespace ssc
{

namespace utils
{
// --------------------------------------------------------


Frame3D::Frame3D()
{
}

Frame3D::~Frame3D()
{
}

//Frame3D Frame3D::create(const Transform3D& transform)
//{
//	return Frame3D();
//}

//Transform3D Frame3D::transform() const
//{
//	return Transform3D();
//}



//void CGSimpleRotationAxis::SetRotationAxis(CPoint3DH K)
//{
//  ThetaXY = K.ThetaXY();
//  ThetaZ = K.ThetaZ();
//}
////---------------------------------------------------------------------------
//

namespace { // unnamed

/**Constrain the input to the range |min,max|
 */	
double constrainValue(double val, double min, double max)
{
	if (val<=min)
		return min;
	if (val>=max)
		return max;
	return val;			
}
double safe_sqrt(double x)
{
	return sqrt(x);
}
int sign(double x)
{
	if (x>=0)
		return 1;
	return -1;
}

} // namespace unnamed

//
// Convert from a rotation matrix T to Frame.
// Translational components of T are extracted directly, i.e. AxisPos and
// such stuff is not extracted.
// Use defAxis when solution is indeterminate(Phi->0)
// and to choose the correct sign(Phi) (use the K in the same half-space
// as defAxis). See Craig p52-53.
Frame3D Frame3D::create(const Transform3D& T)
{
	// There has been problems with a Phi=1E-8 when the off-diagonal elements of
	// T are near-zero. Avoid this by increasing small_limit. It should still be
	// significantly smaller than 0.001 degrees, which is the approx HW resolution.
	static const double MY_SMALL_LIMIT = 1.0E-6;

	Frame3D retVal;

	double Arg = (T[0][0]+T[1][1]+T[2][2]-1.0)/2.0;
	Arg = constrainValue(Arg, -1.0, 1.0);
	retVal.mPhi = acos(Arg);

	Vector3D K(1,0,0);

	if (fabs(sin(retVal.mPhi)) >= MY_SMALL_LIMIT) // Phi != M_PI * n
	{
		K[0] = T[2][1]-T[1][2];
		K[1] = T[0][2]-T[2][0];
		K[2] = T[1][0]-T[0][1];
		K /= 2.0*sin(retVal.mPhi);
		//CGAssert(K.Length()>0.5);
	}
	else if (similar(cos(retVal.mPhi), -1.0)) // Phi == M_PI + 2*M_PI*n
	{ // Evaluate the diagonal of eq. 2.80, insert cos(M_PI)=-1
		K[0] = safe_sqrt((T[0][0] + 1.0) / 2.0);
		K[1] = safe_sqrt((T[1][1] + 1.0) / 2.0);
		K[2] = safe_sqrt((T[2][2] + 1.0) / 2.0);

		// evaluate off-diagonal elements and extract the sign relations.
		if (!similar(T[0][1], 0.0))
			K[1] *= sign(T[0][1]);
		if (!similar(T[0][2], 0.0))
			K[2] *= sign(T[0][2]);
		if (!similar(T[1][2], 0.0))
			K[2] *= sign(T[1][2]);
	}

//	if (dot(K, defAxis) < 0.0)
//	{
//		K *= -1.0;
//		retVal.mPhi *= -1.0;
//	}

	retVal.setRotationAxis(K);
	retVal.mPos = Vector3D(T[0][3], T[1][3], T[2][3]);

	return retVal;
}
//---------------------------------------------------------------------------

Transform3D Frame3D::transform() const
{
	Transform3D RK = generateRotationMatrix();

	// apply translation
	RK[0][3] = mPos[0];
	RK[1][3] = mPos[1];
	RK[2][3] = mPos[2];

	return RK;
}
//
//vtkMatrix4x4Ptr M = space.m_rMs.matrix();
//hdr << M->GetElement(0,0) << '\t' << M->GetElement(0,1) << '\t' << M->GetElement(0,2) << '\t' << M->GetElement(0,3)<< std::endl;
//hdr << M->GetElement(1,0) << '\t' << M->GetElement(1,1) << '\t' << M->GetElement(1,2) << '\t' << M->GetElement(1,3)<< std::endl;
//hdr << M->GetElement(2,0) << '\t' << M->GetElement(2,1) << '\t' << M->GetElement(2,2) << '\t' << M->GetElement(2,3)<< std::endl;
//hdr << M->GetElement(3,0) << '\t' << M->GetElement(3,1) << '\t' << M->GetElement(3,2) << '\t' << M->GetElement(3,3)<< std::endl;
//
//
//// Find the homogenous transform representing the frame.
//// It is given as T = T_p * T_axispos * R_axis * T_-axispos
//// When axispos is invalid or zero, it is reduced to
////                T = T_p * R_axis
////
//// Refer to GaaS' "3D measurement by rotating an arbitrarily
//// oriented object around an arbitrary axis".
//Matrix44 CGFrame::Transform() const
//{
////  if (!Valid())
////    return Matrix44::Identity();
//
//  Matrix44 RK;
//  GenerateRotationMatrix(&RK);
//
//  // apply translation
//  RK[0][3] = P.x;
//  RK[1][3] = P.y;
//  RK[2][3] = P.z;
//
//  return RK;
//}
////---------------------------------------------------------------------------
//

Vector3D Frame3D::rotationAxis() const
{
	Vector3D e;
	e[0] = cos(mThetaXY);
	e[1] = sin(mThetaXY);
	e[2] = sin(mThetaZ);
	return e;
}

void Frame3D::setRotationAxis(const Vector3D& k)
{
	mThetaXY = atan2(k[1], k[0]);
	mThetaZ = atan2(k[2], sqrt(k[0]*k[0]+k[1]*k[1]));
}

// helper method that generates the rotational part of a 4D matrix
// used by both CGFrame and CGRotationAxis
Transform3D Frame3D::generateRotationMatrix() const
{
	Transform3D RK;
	
//	CPoint3DH K(RotationAxis());
//	CPoint3D k(K.X, K.Y, K.Z);
	Vector3D k = rotationAxis();
	double kx = k[0];
	double ky = k[1];
	double kz = k[2];
	double sf = sin(mPhi);
	double cf = cos(mPhi);
	double vf = 1.0-cos(mPhi);

	RK[0][0] = kx*kx*vf + cf;
	RK[1][0] = kx*ky*vf + kz*sf;
	RK[2][0] = kz*kx*vf - ky*sf;

	RK[0][1] = kx*ky*vf - kz*sf;
	RK[1][1] = ky*ky*vf + cf;
	RK[2][1] = ky*kz*vf + kx*sf;

	RK[0][2] = kz*kx*vf + ky*sf;
	RK[1][2] = ky*kz*vf - kx*sf;
	RK[2][2] = kz*kz*vf + cf;
	
	return RK;
}
//---------------------------------------------------------------------------

std::ostream& operator<<(std::ostream& s, const Frame3D& t)
{
	t.put(s);
	return s;
}

void Frame3D::put(std::ostream& s) const
{
	s << "ThetaXY=" << mThetaXY << ", ThetaZ=" << mThetaZ << ", Phi=" << mPhi << ", Pos=[" << mPos << "]";
}


// --------------------------------------------------------
} // namespace utils
} // namespace ssc
// --------------------------------------------------------

