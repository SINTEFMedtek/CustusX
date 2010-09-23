#include "sscData.h"

#include <QDomDocument>
#include "sscRegistrationTransform.h"


namespace ssc
{
Data::Data() : 
  mUid("DummyUID"), mName("DummyName"), mRegistrationStatus(rsNOT_REGISTRATED)
{
}

Data::Data(const std::string& uid, const std::string& name) :
  mUid(uid), mRegistrationStatus(rsNOT_REGISTRATED), mParentFrame("")
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
Data::Data(const std::string& uid, const std::string& name, const vtkPolyDataPtr& polyData) : 
	mUID(uid), mName(name), mVtkPolyData(polyData)
{
}
*/
Data::~Data()
{}
void Data::setUid(const std::string& uid)
{
	mUid = uid;
}

void Data::setName(const std::string& name)
{
	mName = name;
}

void Data::setFilePath(const std::string& filePath)
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

std::string Data::getUid() const
{
	return mUid;
}

std::string Data::getName() const
{
	return mName;
}

std::string Data::getFilePath() const
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
  return m_rMd_History->getCurrentRegistration();
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

std::string Data::getParentFrame()
{
  return mParentFrame;
}

void Data::setParentFrame(std::string uid)
{
  mParentFrame = uid;
}

void Data::addXml(QDomNode& dataNode)
{
  QDomDocument doc = dataNode.ownerDocument();

  m_rMd_History->addXml(dataNode);

  QDomElement uidNode = doc.createElement("uid");
  uidNode.appendChild(doc.createTextNode(mUid.c_str()));
  dataNode.appendChild(uidNode);

  QDomElement nameNode = doc.createElement("name");
  nameNode.appendChild(doc.createTextNode(mName.c_str()));
  dataNode.appendChild(nameNode);

  QDomElement filePathNode = doc.createElement("filePath");
  filePathNode.appendChild(doc.createTextNode(mFilePath.c_str()));
  dataNode.appendChild(filePathNode);

  QDomElement parentFrameNode = doc.createElement("parentFrame");
  parentFrameNode.appendChild(doc.createTextNode(mParentFrame.c_str()));
  dataNode.appendChild(parentFrameNode);
}

void Data::parseXml(QDomNode& dataNode)
{
  if (dataNode.isNull())
    return;

  if(!dataNode.namedItem("parentFrame").isNull())
    mParentFrame = dataNode.namedItem("parentFrame").toElement().text().toStdString();

  QDomNode registrationHistory = dataNode.namedItem("registrationHistory");
  m_rMd_History->parseXml(registrationHistory);
}



} // namespace ssc
