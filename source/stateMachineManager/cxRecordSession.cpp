#include "cxRecordSession.h"

#include <QDomDocument>
#include <QDomElement>
#include "sscTypeConversions.h"
#include "sscMessageManager.h"
#include "cxStateMachineManager.h"
#include "sscTime.h"

namespace cx
{
RecordSession::RecordSession(double startTime, double stopTime, QString description) :
    mStartTime(startTime),
    mStopTime(stopTime),
    mDescription(description)
{
  mUid = this->getNewUid();
  mDescription.append("_"+mUid+"");
}

RecordSession::~RecordSession()
{}

QString RecordSession::getUid()
{
  return mUid;
}

QString RecordSession::getDescription()
{
  return mDescription;
}

double RecordSession::getStartTime()
{
  return mStartTime;
}

double RecordSession::getStopTime()
{
  return mStopTime;
}

ssc::SessionToolHistoryMap RecordSession::getSessionHistory()
{
  return ssc::toolManager()->getSessionHistory(mStartTime, mStopTime);
}

void RecordSession::addXml(QDomNode& parentNode)
{
  QDomDocument doc = parentNode.ownerDocument();

  parentNode.toElement().setAttribute("uid", mUid);

  QDomElement startNode = doc.createElement("start");
  startNode.appendChild(doc.createTextNode(qstring_cast(mStartTime)));
  parentNode.appendChild(startNode);

  QDomElement stopNode = doc.createElement("stop");
  stopNode.appendChild(doc.createTextNode(qstring_cast(mStopTime)));
  parentNode.appendChild(stopNode);

  QDomElement descriptionNode = doc.createElement("description");
  descriptionNode.appendChild(doc.createTextNode(mDescription));
  parentNode.appendChild(descriptionNode);
}

void RecordSession::parseXml(QDomNode& parentNode)
{
  if (parentNode.isNull())
    return;

  QDomElement base = parentNode.toElement();

  mUid = base.attribute("uid");
  bool ok;
  mStartTime = parentNode.namedItem("start").toElement().text().toInt(&ok);
  mStopTime = parentNode.namedItem("stop").toElement().text().toInt(&ok);
  mDescription = parentNode.namedItem("description").toElement().text();
}

QString RecordSession::getNewUid()
{
  QString retval;
  int max = 0;
  std::vector<RecordSessionPtr> recordsessions = stateManager()->getRecordSessions();
  std::vector<RecordSessionPtr>::iterator iter;
  for (iter = recordsessions.begin(); iter != recordsessions.end(); ++iter)
  {
    max = std::max(max, qstring_cast((*iter)->getUid()).toInt());
  }


//  retval = qstring_cast(max + 1);
  retval = QString("%1").arg(max + 1, 2, 10, QChar('0'));
  retval += "_" + QDateTime::currentDateTime().toString(ssc::timestampSecondsFormat());
  return retval;
}

USAcqRecordSession::USAcqRecordSession(double startTime, double stopTime, QString description) :
    RecordSession(startTime, stopTime, description)
{}

USAcqRecordSession::~USAcqRecordSession()
{}

}//namespace cx
