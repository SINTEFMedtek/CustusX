/*
 * cxSelectRecordSessionStringDataAdapter.cpp
 *
 *  \date Jun 14, 2011
 *      \author christiana
 */

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
QString SelectRecordSessionStringDataAdapter::getValueName() const
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
