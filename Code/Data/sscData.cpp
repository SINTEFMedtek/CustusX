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
void Data::setRegistrationStatus(REGISTRATION_STATUS regStat)
{
	mRegistrationStatus = regStat;
}
void Data::set_rMd(Transform3D rMd)
{
	if (similar(rMd, m_rMd))
	{
		return;
	}
	m_rMd = rMd;
}
std::string Data::getUid() const
{
	return mUid;
}
std::string Data::getName() const
{
	return mName;
}
REGISTRATION_STATUS Data::getRegistrationStatus() const
{
	return mRegistrationStatus;
}
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
