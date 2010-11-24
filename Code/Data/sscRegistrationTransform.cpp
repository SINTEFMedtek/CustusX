#include "sscRegistrationTransform.h"
//#include <QtXML>
#include <QDomElement>
#include "sscTime.h"
#include "sscTypeConversions.h"

namespace ssc
{



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

void RegistrationTransform::addXml(QDomNode& parentNode) const ///< write internal state to node
{
  QDomDocument doc = parentNode.ownerDocument();
  QDomElement base = doc.createElement("registrationTransform");
  parentNode.appendChild(base);

  base.setAttribute("timestamp", mTimestamp.toString(timestampSecondsFormat()));
  base.setAttribute("type", mType);

  base.appendChild(doc.createTextNode("\n"+qstring_cast(mValue)));
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

bool operator<(const RegistrationTransform& lhs, const RegistrationTransform& rhs)
{
  return lhs.mTimestamp < rhs.mTimestamp;
}

bool operator==(const RegistrationTransform& lhs, const RegistrationTransform& rhs)
{
  return similar(lhs.mValue, rhs.mValue, 1.0E-3) && (lhs.mTimestamp==rhs.mTimestamp) && (lhs.mType==rhs.mType);
}

//---------------------------------------------------------
//-------  RegistrationTransform  -------------------------
//---------------------------------------------------------

ParentFrame::ParentFrame()
{

}

ParentFrame::ParentFrame(const QString& uid, const QDateTime& timestamp, const QString& type)
{
  mValue = uid;
  mTimestamp = timestamp;
  mType = type;
}

void ParentFrame::addXml(QDomNode& parentNode) const ///< write internal state to node
{
  QDomDocument doc = parentNode.ownerDocument();
  QDomElement base = doc.createElement("parentFrame");
  parentNode.appendChild(base);

  base.setAttribute("value", mValue);
  base.setAttribute("timestamp", mTimestamp.toString(timestampSecondsFormat()));
  base.setAttribute("type", mType);
}

void ParentFrame::parseXml(QDomNode& dataNode)///< read internal state from node
{
  if (dataNode.isNull())
    return;

  QDomElement base = dataNode.toElement();

  mTimestamp = QDateTime::fromString(base.attribute("timestamp"), timestampSecondsFormat());
  mType = base.attribute("type");
  mValue = base.attribute("value");
}

bool operator<(const ParentFrame& lhs, const ParentFrame& rhs)
{
  return lhs.mTimestamp < rhs.mTimestamp;
}

bool operator==(const ParentFrame& lhs, const ParentFrame& rhs)
{
  return (lhs.mValue==rhs.mValue) && (lhs.mTimestamp==rhs.mTimestamp) && (lhs.mType==rhs.mType);
}

//---------------------------------------------------------
//-------  RegistrationHistory    -------------------------
//---------------------------------------------------------



void RegistrationHistory::addXml(QDomNode& parentNode) const ///< write internal state to node
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
  for (unsigned i=0; i<mParentFrames.size(); ++i)
  {
    mParentFrames[i].addXml(base);
  }
}

void RegistrationHistory::parseXml(QDomNode& dataNode)///< read internal state from node
{
  if (dataNode.isNull())
    return;

  mData.clear();
//std::cout << "RegistrationHistory::parseXml" << std::endl;
  //emit currentChanged();
  //return;
  QString currentTimeRaw = dataNode.namedItem("currentTime").toElement().text();
  QDateTime currentTime = QDateTime::fromString(currentTimeRaw, timestampSecondsFormat());

  // iterate over all data elements
  QDomElement currentElem = dataNode.firstChildElement("registrationTransform");
  for ( ; !currentElem.isNull(); currentElem = currentElem.nextSiblingElement("registrationTransform"))
  {
    RegistrationTransform transform;
    transform.parseXml(currentElem);
    mData.push_back(transform);
  }

  // iterate over all data elements
  currentElem = dataNode.firstChildElement("parentFrame");
  for ( ; !currentElem.isNull(); currentElem = currentElem.nextSiblingElement("parentFrame"))
  {
    ParentFrame transform;
    transform.parseXml(currentElem);
    mParentFrames.push_back(transform);
  }

  std::sort(mData.begin(), mData.end());
  std::sort(mParentFrames.begin(), mParentFrames.end());
  setActiveTime(currentTime); // update cache
}

void RegistrationHistory::clear()
{
  mData.clear();
  mCurrentTime = QDateTime();
  mTransformCache = RegistrationTransform();
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

/**Replace the registration performed at oldTime with the new one.
 *
 */
void RegistrationHistory::updateRegistration(const QDateTime& oldTime, const RegistrationTransform& newTransform)
{
  for (std::vector<RegistrationTransform>::iterator iter=mData.begin(); iter!=mData.end(); ++iter)
  {
    if (iter->mTimestamp != oldTime)
      continue;
    mData.erase(iter);
    break;
  }
  this->addRegistration(newTransform);
}

///**Replace the registration matrix performed at oldTime with the new one.
// *
// */
//void RegistrationHistory::updateRegistration(const QDateTime& oldTime, const Transform3D& newTransform)
//{
//  for (std::vector<RegistrationTransform>::iterator iter=mData.begin(); iter!=mData.end(); ++iter)
//  {
//    if (iter->mTimestamp != oldTime)
//      continue;
//    //mData.erase(iter);
//    iter->mValue = newTransform;
//    break;
//  }
////  this->addRegistration(newTransform);
//}
//
///**Update the registration performed at
// */
//void RegistrationHistory::updateRegistration(const QDateTime& oldTime, const QString parentFrame)
//{
//  for (std::vector<RegistrationTransform>::iterator iter=mData.begin(); iter!=mData.end(); ++iter)
//  {
//    if (iter->mTimestamp != oldTime)
//      continue;
//    iter->mParentFrame = parentFrame;
//    break;
//  }
//}

/**Set a registration transform, overwriting all history.
 * Use this when registration history is not needed.
 */
void RegistrationHistory::setRegistration(const Transform3D& transform)
{
  mData.clear();
  this->addRegistration(RegistrationTransform(transform));
}


void RegistrationHistory::addParentFrame(const QString& newParent)
{
  this->addParentFrame(ParentFrame(newParent, QDateTime::currentDateTime(), "Set Parent Frame"));
}

void RegistrationHistory::addParentFrame(const ParentFrame& newParent)
{
  if (std::count(mParentFrames.begin(), mParentFrames.end(), newParent)) // ignore if already present
    return;

  mParentFrames.push_back(newParent);
  std::sort(mParentFrames.begin(), mParentFrames.end());
  setActiveTime(QDateTime()); // reset to last registration when reregistering.
}

void RegistrationHistory::updateParentFrame(const QDateTime& oldTime, const ParentFrame& newParent)
{
  for (std::vector<ParentFrame>::iterator iter=mParentFrames.begin(); iter!=mParentFrames.end(); ++iter)
  {
    if (iter->mTimestamp != oldTime)
      continue;
    mParentFrames.erase(iter);
    break;
  }
  this->addParentFrame(newParent);
}

ParentFrame RegistrationHistory::getCurrentParentFrame()
{
  return mParentFrameCache;
}

std::vector<RegistrationTransform> RegistrationHistory::getData() const
{
  return mData;
}

std::vector<ParentFrame> RegistrationHistory::getParentFrames() const
{
  return mParentFrames;
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

  for (std::vector<ParentFrame>::iterator iter=mParentFrames.begin(); iter!=mParentFrames.end(); )
  {
    if (iter->mTimestamp >= timestamp)
    {
      std::cout << "RegistrationHistory::removeNewerThan("<< timestamp.toString(timestampSecondsFormatNice()) <<"): removed parent frame [" << iter->mTimestamp.toString(timestampSecondsFormatNice()) << ", " << iter->mType << "]" << std::endl;
      iter = mParentFrames.erase(iter);
    }
    else
    {
      ++iter;
    }
  }

  setActiveTime(QDateTime());
}

void RegistrationHistory::setCache(const RegistrationTransform& val, const ParentFrame& parent, const QDateTime& timestamp)
{
  if ((mTransformCache==val) && (mParentFrameCache==parent) && (mCurrentTime==timestamp))
    return;

  mCurrentTime = timestamp;
  mTransformCache = val;
  mParentFrameCache = parent;

//  std::cout << "--------------------------------" << std::endl;
//  std::cout << "mTransformCache\n" << mTransformCache << std::endl;
//  std::cout << streamXml2String(*this) << std::endl;
//  std::cout << "--------------------------------" << std::endl;

//  std::cout << "RegistrationHistory::setCache() " << this << std::endl;

  emit currentChanged();
}

/**set the active time. Use only registrations performed at or prior
 * to this time.
 */
void RegistrationHistory::setActiveTime(const QDateTime& timestamp)
{
//  std::cout << "RegistrationHistory::setActiveTime " << this << std::endl;

  RegistrationTransform val;
  ParentFrame parent;

//  if (mData.empty())
//  {
//    val = RegistrationTransform();
//////    std::cout << "setActiveTime1\n" << Transform3D() << std::endl;
////    setCache(RegistrationTransform(), ParentFrame(), timestamp);
////    return;
//  }

//  // set to running time
//  if (!timestamp.isValid())
//  {
////    std::cout << "--------------------------------" << std::endl;
////    std::cout << "setActiveTime2\n" << mData.back().mValue << std::endl;
////    std::cout << "mData.size() " << mData.size() << std::endl;
////    std::cout << "mData.front().mTimestamp < mData.back().mTimestamp: " << (mData.front().mTimestamp < mData.back().mTimestamp) << std::endl;
////    std::cout << "mData.front() < mData.back(): " << (mData.front() < mData.back()) << std::endl;
////     std::cout << streamXml2String(*this) << std::endl;
////    std::cout << "--------------------------------" << std::endl;
//
//    setCache(mData.back(), timestamp);
//    return;
//  }

//  std::cout << "setActiveTime3" << std::endl;
  // set to specified time
  //RegistrationTransform val;
  if (timestamp.isValid())
  {
    for (std::vector<RegistrationTransform>::iterator iter=mData.begin(); iter!=mData.end(); ++iter)
    {
      if (iter->mTimestamp <= timestamp)
        val = *iter;
    }
    for (std::vector<ParentFrame>::iterator iter=mParentFrames.begin(); iter!=mParentFrames.end(); ++iter)
    {
      if (iter->mTimestamp <= timestamp)
        parent = *iter;
    }
  }
  else
  {
    if (!mData.empty())
      val = mData.back();
    if (!mParentFrames.empty())
      parent = mParentFrames.back();
  }

//  // debug:
//  ///
//  if (!mData.empty())
//  {
//    RegistrationTransform head = mData.back();
//    if (!(head==val))
//    {
//      std::cout << "cache value differ from head" << std::endl;
//    }
//    if (mData.size()==1)
//    {
//      std::cout << "mData.size==1" << std::endl;
//      std::cout << "head: " << streamXml2String(head) << std::endl;
//      std::cout << "val: " << streamXml2String(val) << std::endl;
//    }
//  }
//  ///

  setCache(val, parent, timestamp);
}

QDateTime RegistrationHistory::getActiveTime() const
{
  return mCurrentTime;
}

RegistrationTransform RegistrationHistory::getCurrentRegistration() const
{
  return mTransformCache;
}



} // end namespace ssc

