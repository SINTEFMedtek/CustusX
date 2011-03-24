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
  mTemporalCalibration(0)
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
}

}
