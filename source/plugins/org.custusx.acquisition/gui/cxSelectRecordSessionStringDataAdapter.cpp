/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors 
   may be used to endorse or promote products derived from this software 
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
=========================================================================*/
#include "cxSelectRecordSessionStringDataAdapter.h"
#include "cxTypeConversions.h"

namespace cx
{


SelectRecordSessionStringDataAdapter::SelectRecordSessionStringDataAdapter(AcquisitionDataPtr pluginData) :
		SelectRecordSessionStringDataAdapterBase(pluginData)
{
  connect(mPluginData.get(), SIGNAL(recordedSessionsChanged()), this, SLOT(setDefaultSlot()));
  this->setDefaultSlot();
}
QString SelectRecordSessionStringDataAdapter::getDisplayName() const
{
  return "Select a record session";
}
bool SelectRecordSessionStringDataAdapter::setValue(const QString& value)
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
QString SelectRecordSessionStringDataAdapter::getValue() const
{
  if(!mRecordSession)
    return "<no session>";
  return mRecordSession->getUid();
}
QString SelectRecordSessionStringDataAdapter::getHelp() const
{
  return "Select a session";
}
RecordSessionPtr SelectRecordSessionStringDataAdapter::getRecordSession()
{
  return mRecordSession;
}
void SelectRecordSessionStringDataAdapter::setDefaultSlot()
{
  std::vector<RecordSessionPtr> sessions = mPluginData->getRecordSessions();
	if(!sessions.empty())
    this->setValue(sessions.at(0)->getUid());
}


//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------




SelectRecordSessionStringDataAdapterBase::SelectRecordSessionStringDataAdapterBase(AcquisitionDataPtr pluginData) :
		mPluginData(pluginData)
{
  connect(mPluginData.get(), SIGNAL(recordedSessionsChanged()), this, SIGNAL(changed()));
}
QStringList SelectRecordSessionStringDataAdapterBase::getValueRange() const
{
  std::vector<RecordSessionPtr> sessions =  mPluginData->getRecordSessions();
  QStringList retval;
  retval << "";
  for (unsigned i=0; i<sessions.size(); ++i)
    retval << qstring_cast(sessions[i]->getUid());
  return retval;
}
QString SelectRecordSessionStringDataAdapterBase::convertInternal2Display(QString internal)
{
  RecordSessionPtr session = mPluginData->getRecordSession(internal);
  if(!session)
  {
    return "<no session>";
  }
  return qstring_cast(session->getDescription());
}

}
