#include "sscFrame3D.h"
#include <math.h>
#include "sscUtilHelpers.h"

#include "cml/cml.h"
// --------------------------------------------------------
namespace ssc
{


DecomposedTransform3D::DecomposedTransform3D()
{
  mAngle = Vector3D(0,0,0);
  mPos = Vector3D(0,0,0);
}

DecomposedTransform3D::DecomposedTransform3D(Transform3D m)
{
  ssc::Frame3D frame = Frame3D::create(m);
  mAngle = frame.getEulerXYZ();
  mPos = frame.mPos;
  frame.mPos = Vector3D(0,0,0);
  m_R = frame.transform();
}

void DecomposedTransform3D::reset(Transform3D m)
{
  DecomposedTransform3D input(m);

  bool eqPos = similar(input.mPos, mPos);
  if (!eqPos)
  {
    mPos = input.mPos;
//    std::cout << "reset set pos " << mPos << std::endl;
  }

  input.mPos = mPos;
  bool eqRot = similar(input.getMatrix(), this->getMatrix());
  // only reset angles if the input rotation matrix is different from the current.
  if (!eqRot)
  {
    mAngle = input.mAngle;
    m_R = input.m_R;
//    std::cout << "reset set rot " << mAngle << std::endl;
  }
}

void DecomposedTransform3D::setAngles(Vector3D xyz)
{
//  std::cout << "setAngles " << xyz << std::endl;

  if (!similar(xyz[0], mAngle[0]))
  {
    m_R = m_R * createTransformRotateX(xyz[0]-mAngle[0]);
    mAngle[0] = xyz[0];
  }
  if (!similar(xyz[1], mAngle[1]))
  {
    m_R = m_R * createTransformRotateY(xyz[1]-mAngle[1]);
    mAngle[1] = xyz[1];
  }
  if (!similar(xyz[2], mAngle[2]))
  {
    m_R = m_R * createTransformRotateZ(xyz[2]-mAngle[2]);
    mAngle[2] = xyz[2];
  }
}

void DecomposedTransform3D::setPosition(Vector3D pos)
{
//  std::cout << "setPosition " << pos << std::endl;
  mPos = pos;
}

Vector3D DecomposedTransform3D::getAngles() const
{
  return mAngle;
}

Vector3D DecomposedTransform3D::getPosition() const
{
  return mPos;
}

Transform3D DecomposedTransform3D::getMatrix() const
{
  return createTransformTranslate(mPos) * m_R;
}

//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------


//TODO: are vtk col or row order? must do the same here.
typedef cml::vector< double, cml::fixed<3> > cml_vector_3;
typedef cml::matrix<double, cml::fixed<3,3>, cml::col_basis> cml_matrix_3x3;
typedef cml::matrix<double, cml::fixed<4,4>, cml::col_basis> cml_matrix_4x4;
typedef cml::quaternion<double, cml::fixed<>, cml::scalar_first> cml_quaternion_type;


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

cml_matrix_4x4 convertToCml(const Transform3D& T)
{
  cml_matrix_4x4 m;
  for (int r=0; r<4; ++r)
    for (int c=0; c<4; ++c)
      m(r,c) = T[r][c];
  return m;
}

static cml::EulerOrder mEulerOrder = cml::euler_order_xyz;
//static cml::EulerOrder mEulerOrder = cml::euler_order_zyx;

void cml_test(const Transform3D& T)
{
  cml_matrix_4x4 m = convertToCml(T);

  cml_quaternion_type q;
  cml::quaternion_rotation_matrix(q,m);

  cml_vector_3 axis;
  double angle;
  cml::quaternion_to_axis_angle(q, axis, angle);

  std::cout << "cml aa: " << axis[0] << " " << axis[1] << " " << axis[2] << " " << angle << std::endl;
}

ssc::Vector3D Frame3D::getEulerXYZ() const
{
  cml_quaternion_type q;
  cml_vector_3 axis(rotationAxis()[0], rotationAxis()[1], rotationAxis()[2]);
  cml::quaternion_rotation_axis_angle(q, axis, mPhi);

//  cml_matrix_4x4 m = convertToCml(this->transform());
//
//  cml_quaternion_type q;
//  cml::quaternion_rotation_matrix(q,m);
  ssc::Vector3D retval;

  cml::quaternion_to_euler(q, retval[0], retval[1], retval[2], mEulerOrder);

//  std::cout << "Frame3D::getEulerXYZ qua: " << q[0] << " " << q[1] << " " << q[2] << " " << q[3] << std::endl;
//  std::cout << "Frame3D::getEulerXYZ axi: " << this->rotationAxis() << " " << mPhi << std::endl;
//  std::cout << "Frame3D::getEulerXYZ xyz: " << retval << std::endl;

  return retval;
}

void Frame3D::setEulerXYZ(const ssc::Vector3D& xyz)
{
  cml_quaternion_type q;

//  ssc::Vector3D a = xyz;
//  cml::quaternion_rotation_world_y(q, 0.0);
//  cml::quaternion_rotate_about_world_x(q, a[0]);
//  cml::quaternion_rotate_about_world_y(q, a[1]);
//  cml::quaternion_rotate_about_world_z(q, a[2]);

  cml::quaternion_rotation_euler(q,xyz[0],xyz[1],xyz[2],mEulerOrder);

  cml_vector_3 axis;
  double angle;
  cml::quaternion_to_axis_angle(q, axis, angle);

//  std::cout << "Frame3D::setEulerXYZ xyz: " << xyz << std::endl;
//  std::cout << "Frame3D::setEulerXYZ qua: " << q[0] << " " << q[1] << " " << q[2] << " " << q[3] << std::endl;
//  std::cout << "Frame3D::setEulerXYZ axi: " << axis[0] << " " << axis[1] << " " << axis[2] << " " << angle << std::endl;

  mPhi = angle;
  this->setRotationAxis(ssc::Vector3D(axis[0],axis[1],axis[2]));
}

void testProps(const Transform3D& T)
{
  Frame3D f = Frame3D::create(T);
  std::cout << "T: " << std::endl << T << std::endl;
  ssc::Vector3D k = f.rotationAxis();
  std::cout << "frame aa: " << k[0] << " " << k[1] << " " << k[2] << " " << f.mPhi << std::endl;
  cml_test(T);
}

void Frame3D::test()
{
  cml_quaternion_type q;
  ssc::Vector3D a(30,40,50);
  a = a*M_PI/180;
  cml::quaternion_rotation_world_y(q, 0.0);
  cml::quaternion_rotate_about_world_x(q, a[0]);
  cml::quaternion_rotate_about_world_y(q, a[1]);
  cml::quaternion_rotate_about_world_z(q, a[2]);

  cml::quaternion_to_euler(q, a[0], a[1], a[2], cml::euler_order_xyz);
  a = a/M_PI*180;
  std::cout << "test angles xyz: " << a << std::endl;
  cml::quaternion_to_euler(q, a[0], a[1], a[2], cml::euler_order_zyx);
  a = a/M_PI*180;
  std::cout << "test angles zyx: " << a << std::endl;

  Transform3D M1;

  testProps(M1);
  testProps(createTransformRotateX(M_PI/3)*createTransformRotateY(M_PI/5));
}

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

