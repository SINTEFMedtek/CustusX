// This file is part of SSC,
// a C++ Library supporting Image Guided Therapy Applications.
//
// Copyright (C) 2008- SINTEF Medical Technology
// Copyright (C) 2008- Sonowand AS
//
// SSC is owned by SINTEF Medical Technology and Sonowand AS,
// hereafter named the owners. Each particular piece of code
// is owned by the part that added it to the library.
// SSC source code and binaries can only be used by the owners
// and those with explicit permission from the owners.
// SSC shall not be distributed to anyone else.
//
// SSC is distributed WITHOUT ANY WARRANTY; without even
// the implied warranty of MERCHANTABILITY or FITNESS FOR
// A PARTICULAR PURPOSE.
//
// See sscLicense.txt for more information.

#include "sscProbeData.h"
#include <QDomNode>
#include "sscTypeConversions.h"
#include <iostream>

namespace ssc
{

ProbeData::ProbeData() : mType(tNONE)
{
}

ProbeData::ProbeData(TYPE type, double depthStart, double depthEnd, double width) :
	mType(type), mDepthStart(depthStart), mDepthEnd(depthEnd), mWidth(width),
	mTemporalCalibration(0), mCenterOffset(0)
{
}

ProbeData::ProbeImageData::ProbeImageData() :
	mOrigin_p(0, 0, 0), mSpacing(-1, -1, -1), mClipRect_p(0, 0, 0, 0), mSize(0, 0)
{
}

Vector3D ProbeData::ProbeImageData::transform_p_to_u(const Vector3D& q_p) const
{
  ssc::Vector3D c(q_p[0], double(mSize.height()) - q_p[1] - 1, 0);
  c = multiply_elems(c, mSpacing);
  return c;
}

Vector3D ProbeData::ProbeImageData::getOrigin_u() const
{
  return this->transform_p_to_u(mOrigin_p);
//  ssc::Vector3D c(mOrigin_p[0], double(mSize.height()) - mOrigin_p[1] - 1, 0);
//  c = multiply_elems(c, mSpacing);
//  return c;
}

ssc::DoubleBoundingBox3D ProbeData::ProbeImageData::getClipRect_u() const
{
  ssc::Vector3D p0 = transform_p_to_u(mClipRect_p.corner(0,0,0));
  ssc::Vector3D p1 = transform_p_to_u(mClipRect_p.corner(1,1,1));
//  std::cout << p0 <<  " -- " << p1 << std::endl;
  return ssc::DoubleBoundingBox3D(p0,p1);
}

void ProbeData::addXml(QDomNode& dataNode) const
{
  QDomElement elem = dataNode.toElement();
  elem.setAttribute("type", qstring_cast(mType));
  elem.setAttribute("depthStart", qstring_cast(mDepthStart));
  elem.setAttribute("depthEnd", qstring_cast(mDepthEnd));
  elem.setAttribute("width", qstring_cast(mWidth));
  elem.setAttribute("origin_p", qstring_cast(mImage.mOrigin_p));
  elem.setAttribute("spacing", qstring_cast(mImage.mSpacing));
  elem.setAttribute("size", qstring_cast(mImage.mSize.width())+" "+qstring_cast(mImage.mSize.height()));
  elem.setAttribute("temporalCalibration", qstring_cast(mTemporalCalibration));
  elem.setAttribute("centerOffset", qstring_cast(mCenterOffset));
}

}
