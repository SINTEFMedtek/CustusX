#include "sscFrame3D.h"
#include <math.h>
#include "sscUtilHelpers.h"

namespace ssc
{

DecomposedTransform3D::DecomposedTransform3D()
{
	m_R = Transform3D::Identity();
	mAngle = Vector3D(0, 0, 0);
	mPos = Vector3D(0, 0, 0);
}

DecomposedTransform3D::DecomposedTransform3D(Transform3D m)
{
	ssc::Frame3D frame = Frame3D::create(m);
	mAngle = frame.getEulerXYZ();
	mPos = frame.mPos;
	frame.mPos = Vector3D(0, 0, 0);
	m_R = frame.transform();
}

void DecomposedTransform3D::reset(Transform3D m)
{
	DecomposedTransform3D input(m);

	bool eqPos = similar(input.mPos, mPos);
	if (!eqPos)
	{
		mPos = input.mPos;
	}

	input.mPos = mPos;
	bool eqRot = similar(input.getMatrix(), this->getMatrix());
	// only reset angles if the input rotation matrix is different from the current.
	if (!eqRot)
	{
		mAngle = input.mAngle;
		m_R = input.m_R;
	}
}

void DecomposedTransform3D::setAngles(Vector3D xyz)
{
	//  std::cout << "setAngles " << xyz << std::endl;

	if (!similar(xyz[0], mAngle[0]))
	{
		m_R = m_R * createTransformRotateX(xyz[0] - mAngle[0]);
		mAngle[0] = xyz[0];
	}
	if (!similar(xyz[1], mAngle[1]))
	{
		m_R = m_R * createTransformRotateY(xyz[1] - mAngle[1]);
		mAngle[1] = xyz[1];
	}
	if (!similar(xyz[2], mAngle[2]))
	{
		m_R = m_R * createTransformRotateZ(xyz[2] - mAngle[2]);
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

Frame3D::Frame3D()
{
	mAngleAxis = Eigen::AngleAxisd::Identity();
	mPos = Vector3D(0, 0, 0);
}

Frame3D::~Frame3D()
{
}

ssc::Vector3D Frame3D::getEulerXYZ() const
{
	Transform3D t;
	t = mAngleAxis;
	Vector3D ea = t.matrix().block<3, 3> (0, 0).eulerAngles(0, 1, 2);
	return ea;
}

void Frame3D::setEulerXYZ(const ssc::Vector3D& xyz)
{
	mAngleAxis = Eigen::AngleAxisd(xyz[0], Vector3D::UnitX()) * Eigen::AngleAxisd(xyz[1], Vector3D::UnitY())
		* Eigen::AngleAxisd(xyz[2], Vector3D::UnitZ());
}

/**Create from affine matrix
 *
 */
Frame3D Frame3D::create(const Transform3D& T)
{
	Frame3D retval;

	Eigen::Matrix3d R = T.matrix().block<3, 3> (0, 0); // extract rotational part
	retval.mAngleAxis = Eigen::AngleAxisd(R); // construct angle axis from R
	retval.mPos = T.matrix().block<3, 1> (0, 3); // extract translational part

	return retval;
}
//---------------------------------------------------------------------------

Transform3D Frame3D::transform() const
{
	return createTransformTranslate(mPos) * mAngleAxis;
}

Vector3D Frame3D::rotationAxis() const
{
	return mAngleAxis.axis();
}

void Frame3D::setRotationAxis(const Vector3D& k)
{
	mAngleAxis = Eigen::AngleAxisd(mAngleAxis.angle(), k);
	mAngleAxis = Eigen::AngleAxisd(mAngleAxis.angle(), k);
}

std::ostream& operator<<(std::ostream& s, const Frame3D& t)
{
	t.put(s);
	return s;
}

void Frame3D::put(std::ostream& s) const
{
	s << "ThetaXY=" << getThetaXY(mAngleAxis.axis()) / M_PI * 180 << ", ThetaZ=" << getThetaZ(mAngleAxis.axis()) / M_PI
		* 180 << ", Phi=" << mAngleAxis.angle() / M_PI * 180 << ", Pos=[" << mPos << "]";
}

boost::array<double, 6> Frame3D::getCompactAxisAngleRep() const
{
	boost::array<double, 6> retval;
	retval[0] = getThetaXY(mAngleAxis.axis());
	retval[1] = getThetaZ(mAngleAxis.axis());
	retval[2] = mAngleAxis.angle();
	retval[3] = mPos[0];
	retval[4] = mPos[1];
	retval[5] = mPos[2];
	return retval;
}

Frame3D Frame3D::fromCompactAxisAngleRep(const boost::array<double, 6>& rep)
{
	ssc::Frame3D retval;

	retval.mAngleAxis = Eigen::AngleAxisd(rep[2], unitVector(rep[0], rep[1]));
	retval.mPos = Vector3D(rep[3], rep[4], rep[5]);

	return retval;
}

} // namespace ssc
// --------------------------------------------------------

