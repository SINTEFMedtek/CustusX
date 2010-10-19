#include "sscLandmark.h"

#include <QDomNode>

#include "sscTypeConversions.h"
#include "sscTime.h"

namespace ssc
{

Landmark::Landmark(QString uid, Vector3D coord) :
  mUid(uid), mCoord(coord)
{
  mTimestamp = QDateTime::currentDateTime();
}

Landmark::~Landmark()
{}

QString Landmark::getUid() const
{
  return mUid;
}

ssc::Vector3D Landmark::getCoord() const
{
  return mCoord;
}
QDateTime Landmark::getTimestamp() const
{
  return mTimestamp;
}

void Landmark::addXml(QDomNode& dataNode)
{
  QDomDocument doc = dataNode.ownerDocument();

  dataNode.toElement().setAttribute("uid", qstring_cast(mUid));

  QDomElement coordNode = doc.createElement("coord");
  coordNode.appendChild(doc.createTextNode(qstring_cast(mCoord)));
  dataNode.appendChild(coordNode);

  QDomElement timestampNode = doc.createElement("timestamp");
  timestampNode.appendChild(doc.createTextNode(mTimestamp.toString(timestampSecondsFormat())));
  dataNode.appendChild(timestampNode);
}
void Landmark::parseXml(QDomNode& dataNode)
{
  if (dataNode.isNull())
    return;

  QDomElement base = dataNode.toElement();

  mUid = base.attribute("uid");
  mCoord = Vector3D::fromString(dataNode.namedItem("coord").toElement().text());
  mTimestamp = QDateTime::fromString(dataNode.namedItem("timestamp").toElement().text(), timestampSecondsFormat());
}

///--------------------------------------------------------
///--------------------------------------------------------
///--------------------------------------------------------

LandmarkProperty::LandmarkProperty(const QString& uid, const QString& name, bool active) : mUid(uid), mName(name), mActive(active)
{
  if (mName.isEmpty())
    mName = mUid;
}

void LandmarkProperty::setName(const QString& name)
{
  mName = name;
}

void LandmarkProperty::setActive(bool active)
{
  mActive = active;
}

QString LandmarkProperty::getUid() const
{
  return mUid;
}

bool LandmarkProperty::getActive() const
{
  return mActive;
}

QString LandmarkProperty::getName() const
{
  return mName;
}

void LandmarkProperty::addXml(QDomNode& dataNode)
{
  dataNode.toElement().setAttribute("uid", qstring_cast(mUid));
  dataNode.toElement().setAttribute("active", qstring_cast(mActive));
  dataNode.toElement().setAttribute("name", qstring_cast(mName));
}
void LandmarkProperty::parseXml(QDomNode& dataNode)
{
  if (dataNode.isNull())
    return;

  QDomElement base = dataNode.toElement();
  mUid = base.attribute("uid");
  mActive = base.attribute("active").toInt();
  mName = base.attribute("name");
}


} // namespace ssc
