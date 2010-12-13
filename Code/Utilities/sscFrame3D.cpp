#include "sscFrame3D.h"
#include <math.h>
#include "sscUtilHelpers.h"
// --------------------------------------------------------
namespace ssc
{

//namespace utils
//{
//// --------------------------------------------------------


Frame3D::Frame3D()
{
    mThetaXY = 0;
    mThetaZ = 0;
    mPhi = 0;
    mPos = Vector3D(0,0,0);
}

Frame3D::~Frame3D()
{
}

namespace { // unnamed

///**Constrain the input to the range |min,max|
// */
//double constrainValue(double val, double min, double max)
//{
//	if (val<=min)
//		return min;
//	if (val>=max)
//		return max;
//	return val;
//}
double safe_sqrt(double x)
{
	return sqrt(x);
}
//int sign(double x)
//{
//	if (x>=0)
//		return 1;
//	return -1;
//}

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

Vector3D Frame3D::rotationAxis() const
{
	Vector3D e;
	e[0] = cos(mThetaXY)*cos(mThetaZ);
	e[1] = sin(mThetaXY)*cos(mThetaZ);
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
	s << "ThetaXY=" << mThetaXY/M_PI*180 << ", ThetaZ=" << mThetaZ/M_PI*180 << ", Phi=" << mPhi/M_PI*180 << ", Pos=[" << mPos << "]";
}


//// --------------------------------------------------------
//} // namespace utils
} // namespace ssc
// --------------------------------------------------------

