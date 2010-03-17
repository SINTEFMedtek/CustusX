#include "sscRegistrationTransform.h"
#include <QtXML>
#include "sscTypeConversions.h"

namespace ssc
{

namespace
{

QString timestampSecondsFormat()
{
  return QString("yyyyMMdd'T'hhmmss");
}

} // unnamed namespace

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

  QString currentTimeRaw = dataNode.namedItem("currentTime").toElement().text();
  mCurrentTime = QDateTime::fromString(currentTimeRaw, timestampSecondsFormat());

  // iterate over all data elements
  QDomElement currentElem = dataNode.firstChildElement("registrationTransform");
  for ( ; !currentElem.isNull(); currentElem = dataNode.firstChildElement("registrationTransform"))
  {
    mData.push_back(RegistrationTransform());
    mData.back().parseXml(currentElem);
  }

  setActiveTime(mCurrentTime); // update cache
}

/**Add one registration transform to the history.
 */
void RegistrationHistory::addRegistration(const RegistrationTransform& transform)
{
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
  for (std::vector<RegistrationTransform>::iterator iter=mData.begin(); iter!=mData.end(); )
  {
    if (iter->mTimestamp >= timestamp)
    {
      iter = mData.erase(iter);
    }
    else
    {
      ++iter;
    }
  }

  setActiveTime(QDateTime());
}

void RegistrationHistory::setCache(const Transform3D& val)
{
  if (similar(mTransformCache, val))
    return;

  mTransformCache = val;
  emit currentChanged();
}

/**set the active time. Use only registrations performed at or prior
 * to this time.
 */
void RegistrationHistory::setActiveTime(const QDateTime& timestamp)
{
  mCurrentTime = timestamp;

  if (mData.empty())
  {
    setCache(Transform3D());
    return;
  }

  // set to running time
  if (!mCurrentTime.isValid())
  {
    setCache(mData.back().mValue);
    return;
  }

  // set to specified time
  Transform3D val;
  for (std::vector<RegistrationTransform>::iterator iter=mData.begin(); iter!=mData.end(); ++iter)
  {
    if (iter->mTimestamp <= timestamp)
      val = iter->mValue;
  }
  setCache(val);
}

QDateTime RegistrationHistory::getActiveTime() const
{
  return mCurrentTime;
}

Transform3D RegistrationHistory::getCurrentRegistration() const
{
  return mTransformCache;
}

///**collect registration histories from the tool manager (patient registration)
// * and images (image registration) and return.
// */
//std::vector<RegistrationHistory> getAllRegistrationHistories()
//{
//  std::vector<RegistrationHistory> retval;
//  retval.push_back(ssc::ToolManager::getInstance()->get_rMpr_History());
//
//  std::map<std::string, ssc::ImagePtr> images = ssc:DataManager::getInstance()->getImages();
//  for (std::map<std::string, ImagePtr>::iterator iter=image.begin(); iter!=image.end(); ++iter)
//  {
//    retval.push_back(iter->second->get_rMpr_History());
//  }
//
//  return retval;
//}
//
///**Remove the latest registration event (image or patient)
// * inserted into the system.
// */
//void removeLastRegistration()
//{
//  //search among all images and patient reg, find latest, remove that one.
//  std::vector<RegistrationHistoryPtr> raw = getAllRegistrationHistories();
//
//  std::vector<RegistrationTransform> history;
//  for (unsigned i=0; i<raw.size(); ++i)
//  {
//    std::vector<RegistrationTransform> current = raw[i]->getData();
//    std::copy(current.begin(), current.end(), back_insert_iterator(history));
//  }
//  std::sort(history.begin(), history.end());
//
//  if (history.empty())
//    return;
//
//  QDateTime lastTime = history.back().mTimestamp;
//  lastTime.addSecs(-1);
//
//  for (unsigned i=0; i<raw.size(); ++i)
//  {
//    raw[i]->removeNewerThan(lastTime);
//  }
//}
//
///**Take one step back in registration time and use the previous
// * registration event instead of the current.
// */
//void usePreviousRegistration()
//{
//  std::vector<RegistrationHistoryPtr> raw = getAllRegistrationHistories();
//
//  std::vector<RegistrationTransform> history;
//  for (unsigned i=0; i<raw.size(); ++i)
//  {
//    std::vector<RegistrationTransform> current = raw[i]->getData();
//    std::copy(current.begin(), current.end(), back_insert_iterator(history));
//  }
//  std::sort(history.begin(), history.end());
//
//  if (history.empty())
//    return;
//
//  QDateTime lastTime = history.back().mTimestamp;
//  lastTime.addSecs(-1);
//
//  for (unsigned i=0; i<raw.size(); ++i)
//  {
//    raw[i]->setActiveTime(lastTime);
//  }
//}
//
///**Use the newest available registration.
// * Negates any call to usePreviousRegistration.
// */
//void useNewestRegistration()
//{
//  std::vector<RegistrationHistoryPtr> raw = getAllRegistrationHistories();
//
//  for (unsigned i=0; i<raw.size(); ++i)
//  {
//    raw[i]->setActiveTime(QDateTime());
//  }
//}
//
///**Return true if the system is currently using the newest
// * available registration.
// */
//void isUsingNewestRegistration()
//{
//  std::vector<RegistrationHistoryPtr> raw = getAllRegistrationHistories();
//
//  for (unsigned i=0; i<raw.size(); ++i)
//  {
//    if (raw[i]->getActiveTime()!=QDateTime())
//      return false;
//  }
//
//  return true;
//}


} // end namespace ssc

