#include "sscRegistrationTransform.h"
#include <QtXML>
#include "sscTypeConversions.h"

namespace ssc
{


QString timestampSecondsFormat()
{
  return QString("yyyyMMdd'T'hhmmss");
}
QString timestampSecondsFormatNice()
{
  return QString("yyyy-MM-dd hh:mm:ss");
}

template<class T>
QString streamXml2String(T& val)
{
  QDomDocument doc;
  QDomElement root = doc.createElement("root");
  doc.appendChild(root);

  val.addXml(root);
  return doc.toString();
}

//---------------------------------------------------------
//-------  RegistrationTransform  -------------------------
//---------------------------------------------------------

RegistrationTransform::RegistrationTransform()
{

}

RegistrationTransform::RegistrationTransform(const Transform3D& value, const QDateTime& timestamp, const QString& type)
{
  mValue = value;
  mTimestamp = timestamp;
  mType = type;
}

bool RegistrationTransform::operator<(const RegistrationTransform& rhs) const
{
  return mTimestamp < rhs.mTimestamp;
}

bool RegistrationTransform::operator==(const RegistrationTransform& rhs) const
{
  return similar(mValue, rhs.mValue) && mTimestamp==rhs.mTimestamp && mType==rhs.mType;
}

void RegistrationTransform::addXml(QDomNode& parentNode) ///< write internal state to node
{
  QDomDocument doc = parentNode.ownerDocument();
  QDomElement base = doc.createElement("registrationTransform");
  parentNode.appendChild(base);

  base.setAttribute("timestamp", mTimestamp.toString(timestampSecondsFormat()));
  base.setAttribute("type", mType);
  base.appendChild(doc.createTextNode(qstring_cast(mValue)));
}

void RegistrationTransform::parseXml(QDomNode& dataNode)///< read internal state from node
{
  if (dataNode.isNull())
    return;

  QDomElement base = dataNode.toElement();

  mTimestamp = QDateTime::fromString(base.attribute("timestamp"), timestampSecondsFormat());
  mType = base.attribute("type");
  mValue = Transform3D::fromString(base.text());
}


//---------------------------------------------------------
//-------  RegistrationHistory    -------------------------
//---------------------------------------------------------



void RegistrationHistory::addXml(QDomNode& parentNode) ///< write internal state to node
{
  QDomDocument doc = parentNode.ownerDocument();
  QDomElement base = doc.createElement("registrationHistory");
  parentNode.appendChild(base);

  QDomElement currentTime = doc.createElement("currentTime");
  currentTime.appendChild(doc.createTextNode(mCurrentTime.toString(timestampSecondsFormat())));
  base.appendChild(currentTime);

  for (unsigned i=0; i<mData.size(); ++i)
  {
    mData[i].addXml(base);
  }
}

void RegistrationHistory::parseXml(QDomNode& dataNode)///< read internal state from node
{
  if (dataNode.isNull())
    return;

  //emit currentChanged();
  //return;
  QString currentTimeRaw = dataNode.namedItem("currentTime").toElement().text();
  QDateTime currentTime = QDateTime::fromString(currentTimeRaw, timestampSecondsFormat());

  // iterate over all data elements
  QDomElement currentElem = dataNode.firstChildElement("registrationTransform");
  for ( ; !currentElem.isNull(); currentElem = currentElem.nextSiblingElement("registrationTransform"))
  {
    mData.push_back(RegistrationTransform());
    mData.back().parseXml(currentElem);
  }

  std::sort(mData.begin(), mData.end());
  setActiveTime(currentTime); // update cache
}

/**Add one registration transform to the history.
 */
void RegistrationHistory::addRegistration(const RegistrationTransform& transform)
{
  if (std::count(mData.begin(), mData.end(), transform)) // ignore if already present
    return;

  mData.push_back(transform);
  std::sort(mData.begin(), mData.end());
  setActiveTime(QDateTime()); // reset to last registration when reregistering.
}

/**Set a registration transform, overwriting all history.
 * Use this when registration history is not needed.
 */
void RegistrationHistory::setRegistration(const Transform3D& transform)
{
  mData.clear();
  addRegistration(RegistrationTransform(transform));
}

std::vector<RegistrationTransform> RegistrationHistory::getData() const
{
  return mData;
}

void RegistrationHistory::removeNewerThan(const QDateTime& timestamp)
{
  if (!timestamp.isValid())
    return;
  //std::remove_if(mData.begin(), mData.end(), );
//... remove_if etc etc.
  //std::cout << "RegistrationHistory::removeNewerThan("<< timestamp.toString(timestampSecondsFormatNice()) <<")" << std::endl;

  for (std::vector<RegistrationTransform>::iterator iter=mData.begin(); iter!=mData.end(); )
  {
    //std::cout << "  iter->mTimestamp: " << iter->mTimestamp.toString(timestampSecondsFormatNice()) << std::endl;
    //std::cout << "  (iter->mTimestamp >= timestamp): " << (iter->mTimestamp >= timestamp) << std::endl;

    if (iter->mTimestamp >= timestamp)
    {
      std::cout << "RegistrationHistory::removeNewerThan("<< timestamp.toString(timestampSecondsFormatNice()) <<"): removed [" << iter->mTimestamp.toString(timestampSecondsFormatNice()) << ", " << iter->mType << "]" << std::endl;
      iter = mData.erase(iter);
    }
    else
    {
      ++iter;
    }
  }

  setActiveTime(QDateTime());
}

void RegistrationHistory::setCache(const Transform3D& val, const QDateTime& timestamp)
{
  if (similar(mTransformCache, val) && (mCurrentTime==timestamp))
    return;

  mCurrentTime = timestamp;
  mTransformCache = val;

//  std::cout << "--------------------------------" << std::endl;
//  std::cout << "mTransformCache\n" << mTransformCache << std::endl;
//  std::cout << streamXml2String(*this) << std::endl;
//  std::cout << "--------------------------------" << std::endl;

  emit currentChanged();
}

/**set the active time. Use only registrations performed at or prior
 * to this time.
 */
void RegistrationHistory::setActiveTime(const QDateTime& timestamp)
{
  if (mData.empty())
  {
//    std::cout << "setActiveTime1\n" << Transform3D() << std::endl;
    setCache(Transform3D(), timestamp);
    return;
  }

  // set to running time
  if (!timestamp.isValid())
  {
//    std::cout << "--------------------------------" << std::endl;
//    std::cout << "setActiveTime2\n" << mData.back().mValue << std::endl;
//    std::cout << "mData.size() " << mData.size() << std::endl;
//    std::cout << "mData.front().mTimestamp < mData.back().mTimestamp: " << (mData.front().mTimestamp < mData.back().mTimestamp) << std::endl;
//    std::cout << "mData.front() < mData.back(): " << (mData.front() < mData.back()) << std::endl;
//     std::cout << streamXml2String(*this) << std::endl;
//    std::cout << "--------------------------------" << std::endl;

    setCache(mData.back().mValue, timestamp);
    return;
  }

//  std::cout << "setActiveTime3" << std::endl;
  // set to specified time
  Transform3D val;
  for (std::vector<RegistrationTransform>::iterator iter=mData.begin(); iter!=mData.end(); ++iter)
  {
    if (iter->mTimestamp <= timestamp)
      val = iter->mValue;
  }
  setCache(val, timestamp);
}

QDateTime RegistrationHistory::getActiveTime() const
{
  return mCurrentTime;
}

Transform3D RegistrationHistory::getCurrentRegistration() const
{
  return mTransformCache;
}



} // end namespace ssc

