#include "sscData.h"


namespace ssc
{
Data::Data() : 
  mUid("DummyUID"), mName("DummyName"), mRegistrationStatus(rsNOT_REGISTRATED)
{}
Data::Data(const std::string& uid, const std::string& name) :
  mUid(uid), mName(name), mRegistrationStatus(rsNOT_REGISTRATED)
{}
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
void Data::set_rMd(Transform3D rMd)
{
	if (similar(rMd, m_rMd))
	{
		return;
	}
	m_rMd = rMd;
	emit transformChanged();
}
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
	return m_rMd;
}
void Data::connectToRep(const RepWeakPtr& rep)
{
	mReps.insert(rep);
}
void Data::disconnectFromRep(const RepWeakPtr& rep)
{
	mReps.erase(rep);
}
} // namespace ssc
