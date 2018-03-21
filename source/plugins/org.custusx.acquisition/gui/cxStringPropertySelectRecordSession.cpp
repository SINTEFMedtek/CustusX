/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#include "cxStringPropertySelectRecordSession.h"
#include "cxTypeConversions.h"

namespace cx
{


StringPropertySelectRecordSession::StringPropertySelectRecordSession(AcquisitionDataPtr pluginData) :
		StringPropertySelectRecordSessionBase(pluginData)
{
	connect(mPluginData.get(), &AcquisitionData::recordedSessionsChanged, this, &StringPropertySelectRecordSession::setDefaultSlot);
	this->setDefaultSlot();
}
QString StringPropertySelectRecordSession::getDisplayName() const
{
  return "Select a record session";
}
bool StringPropertySelectRecordSession::setValue(const QString& value)
{
  if(mRecordSession && value==mRecordSession->getUid())
    return false;
  RecordSessionPtr temp = mPluginData->getRecordSession(value);
  if(!temp)
    return false;

  mRecordSession = temp;
  emit changed();
  return true;
}
QString StringPropertySelectRecordSession::getValue() const
{
  if(!mRecordSession)
    return "<no session>";
  return mRecordSession->getUid();
}
QString StringPropertySelectRecordSession::getHelp() const
{
  return "Select a session";
}
RecordSessionPtr StringPropertySelectRecordSession::getRecordSession()
{
  return mRecordSession;
}
void StringPropertySelectRecordSession::setDefaultSlot()
{
  std::vector<RecordSessionPtr> sessions = mPluginData->getRecordSessions();
	if(!sessions.empty())
    this->setValue(sessions.at(0)->getUid());
}


//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------




StringPropertySelectRecordSessionBase::StringPropertySelectRecordSessionBase(AcquisitionDataPtr pluginData) :
		mPluginData(pluginData)
{
	connect(mPluginData.get(), &AcquisitionData::recordedSessionsChanged, this, &StringPropertySelectRecordSessionBase::changed);
}
QStringList StringPropertySelectRecordSessionBase::getValueRange() const
{
  std::vector<RecordSessionPtr> sessions =  mPluginData->getRecordSessions();
  QStringList retval;
  retval << "";
  for (unsigned i=0; i<sessions.size(); ++i)
    retval << qstring_cast(sessions[i]->getUid());
  return retval;
}
QString StringPropertySelectRecordSessionBase::convertInternal2Display(QString internal)
{
  RecordSessionPtr session = mPluginData->getRecordSession(internal);
  if(!session)
  {
    return "<no session>";
  }
  return qstring_cast(session->getDescription());
}

}
