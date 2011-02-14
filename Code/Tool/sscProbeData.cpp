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

void ProbeData::addXml(QDomNode& dataNode) const
{
  QDomElement elem = dataNode.toElement();
  elem.setAttribute("type", qstring_cast(mType));
  elem.setAttribute("depthStart", qstring_cast(mDepthStart));
  elem.setAttribute("depthEnd", qstring_cast(mDepthEnd));
  elem.setAttribute("width", qstring_cast(mWidth));
  elem.setAttribute("origin_u", qstring_cast(mImage.mOrigin_u));
  elem.setAttribute("spacing", qstring_cast(mImage.mSpacing));
  elem.setAttribute("size", qstring_cast(mImage.mSize.width())+" "+qstring_cast(mImage.mSize.height()));
  elem.setAttribute("temporalCalibration", qstring_cast(mTemporalCalibration));
}

}
