#include "sscData.h"

#include <QDomDocument>
#include <QDateTime>
#include <QRegExp>
#include "sscRegistrationTransform.h"
#include "sscTime.h"

namespace ssc
{
Data::Data() : 
  mUid("DummyUID"), mName("DummyName"), mRegistrationStatus(rsNOT_REGISTRATED)
{
}

Data::Data(const QString& uid, const QString& name) :
  mUid(uid), mRegistrationStatus(rsNOT_REGISTRATED)//, mParentFrame("")
{
  if(name=="")
    mName = mUid;
  else
    mName = name;
  m_rMd_History.reset(new ssc::RegistrationHistory() );
  connect(m_rMd_History.get(), SIGNAL(currentChanged()), this, SIGNAL(transformChanged()));
  connect(m_rMd_History.get(), SIGNAL(currentChanged()), this, SLOT(transformChangedSlot()));
}

Data::~Data()
{}
void Data::setUid(const QString& uid)
{
	mUid = uid;
}

void Data::setName(const QString& name)
{
	mName = name;
}

void Data::setFilePath(const QString& filePath)
{
  mFilePath = filePath;
}

void Data::setRegistrationStatus(REGISTRATION_STATUS regStat)
{
	mRegistrationStatus = regStat;
}

QString Data::getUid() const
{
	return mUid;
}

QString Data::getName() const
{
	return mName;
}

QString Data::getFilePath() const
{
  return mFilePath;
}

REGISTRATION_STATUS Data::getRegistrationStatus() const
{
	return mRegistrationStatus;
}
/**
 * @return Transform from local data space to (data-)ref space
 */
Transform3D Data::get_rMd() const
{
  return m_rMd_History->getCurrentRegistration().mValue;
}
void Data::connectToRep(const RepWeakPtr& rep)
{
	mReps.insert(rep);
}
void Data::disconnectFromRep(const RepWeakPtr& rep)
{
	mReps.erase(rep);
}
RegistrationHistoryPtr Data::get_rMd_History()
{
  return m_rMd_History;
}

void Data::setShading(bool on)
{
}

bool Data::getShading() const
{
  return false;
}

QString Data::getSpace()
{
	return mUid;
}

QString Data::getParentSpace()
{
  return m_rMd_History->getCurrentParentSpace().mValue;
}

void Data::addXml(QDomNode& dataNode)
{
  QDomDocument doc = dataNode.ownerDocument();

  m_rMd_History->addXml(dataNode);

  dataNode.toElement().setAttribute("uid", mUid);
  dataNode.toElement().setAttribute("name", mName);
  dataNode.toElement().setAttribute("type", this->getType());

  QDomElement filePathNode = doc.createElement("filePath");
  filePathNode.appendChild(doc.createTextNode(mFilePath));
  dataNode.appendChild(filePathNode);

  QDomElement acqTimeNode = doc.createElement("acqusitionTime");
  acqTimeNode.appendChild(doc.createTextNode(mAcquisitionTime.toString(timestampMilliSecondsFormat())));
  dataNode.appendChild(acqTimeNode);
}

void Data::parseXml(QDomNode& dataNode)
{
  if (dataNode.isNull())
    return;

  QDomNode registrationHistory = dataNode.namedItem("registrationHistory");
  m_rMd_History->parseXml(registrationHistory);

  if (!dataNode.namedItem("acqusitionTime").toElement().isNull())
    mAcquisitionTime = QDateTime::fromString(dataNode.namedItem("acqusitionTime").toElement().text(), timestampMilliSecondsFormat());
}

/**Get the time the data was created from a data source.
 *
 */
QDateTime Data::getAcquisitionTime() const
{
  if (!mAcquisitionTime.isNull())
    return mAcquisitionTime;
  // quickie implementation: Assume uid contains time on format timestampSecondsFormat():

  // retrieve timestamp as
  QRegExp tsReg("[0-9]{8}T[0-9]{6}");
  if (tsReg.indexIn(mUid)>0)
  {
    QDateTime datetime = QDateTime::fromString(tsReg.cap(0), timestampSecondsFormat());
    return datetime;
  }
  return QDateTime();
}

void Data::setAcquisitionTime(QDateTime time)
{
  mAcquisitionTime = time;
}


} // namespace ssc
