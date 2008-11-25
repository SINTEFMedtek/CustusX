#include "sscData.h"


namespace ssc
{


Data::Data() : 
	mUID("DummyUID"), mName("DummyName")
{
}

/*
Data::Data(const std::string& uid, const std::string& name, const vtkPolyDataPtr& polyData) : 
	mUID(uid), mName(name), mVtkPolyData(polyData)
{
}
*/

Data::~Data()
{
}

//-----
void Data::setUID(const std::string& uid)
{
	mUID = uid;
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
	m_rMd = rMd;
}


//--
std::string Data::getUID() const
{
	return mUID;
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


//-----
void Data::connectToRep(const RepWeakPtr& rep)
{
	mReps.insert(rep);
}

void Data::disconnectFromRep(const RepWeakPtr& rep)
{
	mReps.erase(rep);
}

} // namespace ssc
