#include "sscProbeData.h"
#include <QDomNode>
#include "sscTypeConversions.h"

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

Vector3D ProbeData::ProbeImageData::getOrigin_u() const
{
  ssc::Vector3D c(mOrigin_p[0], double(mSize.height()) - mOrigin_p[1] - 1, 0);
  c = multiply_elems(c, mSpacing);
  return c;
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
