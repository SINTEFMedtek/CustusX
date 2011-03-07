#include "sscData.h"

#include <QDomDocument>
#include "sscRegistrationTransform.h"


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
/*
Data::Data(const QString& uid, const QString& name, const vtkPolyDataPtr& polyData) : 
	mUID(uid), mName(name), mVtkPolyData(polyData)
{
}
*/
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

/**
 * Set the transform that brings a point from local data space to (data-)ref space
 * Emits the transformChanged() signal.
 * @param rMd the transformation from data to ref
 */
/*void Data::set_rMd(Transform3D rMd)
{
  m_rMd_History->setRegistration(rMd);
//	if (similar(rMd, m_rMd))
//	{
//		return;
//	}
//	m_rMd = rMd;
//	emit transformChanged();
}*/

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

QString Data::getParentFrame()
{
  return m_rMd_History->getCurrentParentFrame().mValue;
//  return mParentFrame;
}

//void Data::setParentFrame(QString uid)
//{
//  RegistrationTransform transform = m_rMd_History->getCurrentRegistration();
//  transform.mParentFrame = uid;
//  transform.mType = "Set Parent Frame";
//  transform.mTimestamp = QDateTime::currentDateTime();
//  m_rMd_History->addRegistration(transform);
//
//  //mParentFrame = uid;
//  //emit transformChanged();
//}

void Data::addXml(QDomNode& dataNode)
{
  QDomDocument doc = dataNode.ownerDocument();

  m_rMd_History->addXml(dataNode);

//  QDomElement uidNode = doc.createElement("uid");
//  uidNode.appendChild(doc.createTextNode(mUid));
//  dataNode.appendChild(uidNode);

  dataNode.toElement().setAttribute("uid", mUid);
  dataNode.toElement().setAttribute("name", mName);
//  QDomElement nameNode = doc.createElement("name");
//  nameNode.appendChild(doc.createTextNode(mName));
//  dataNode.appendChild(nameNode);

  QDomElement filePathNode = doc.createElement("filePath");
  filePathNode.appendChild(doc.createTextNode(mFilePath));
  dataNode.appendChild(filePathNode);

//  QDomElement parentFrameNode = doc.createElement("parentFrame");
//  parentFrameNode.appendChild(doc.createTextNode(mParentFrame));
//  dataNode.appendChild(parentFrameNode);
}

void Data::parseXml(QDomNode& dataNode)
{
  if (dataNode.isNull())
    return;

//  // backward compatibility per 20101117: remove sometime in 2011
//  QString parentFrame;
//  if(!dataNode.namedItem("parentFrame").isNull())
//    parentFrame = dataNode.namedItem("parentFrame").toElement().text();

  QDomNode registrationHistory = dataNode.namedItem("registrationHistory");
  m_rMd_History->parseXml(registrationHistory);

//  if (!parentFrame.isEmpty())
//  {
//    m_rMd_History->addParentFrame(parentFrame);
////    this->setParentFrame(parentFrame);
//  }
}



} // namespace ssc
