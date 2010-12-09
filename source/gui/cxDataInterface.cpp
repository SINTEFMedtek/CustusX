/*
 * cxDataInterface.cpp
 *
 *  Created on: Apr 13, 2010
 *      Author: christiana
 */
#include "cxDataInterface.h"
#include "sscImage.h"
#include "sscMesh.h"
#include "sscMessageManager.h"
#include "sscImageLUT2D.h"
#include "sscDataManager.h"
#include "sscToolManager.h"
#include "sscTypeConversions.h"
#include "sscDefinitionStrings.h"
#include "sscEnumConverter.h"
#include "sscTool.h"
#include "cxStateMachineManager.h"

namespace cx
{

DoubleDataAdapterActiveToolOffset::DoubleDataAdapterActiveToolOffset()
{
  connect(ssc::toolManager(), SIGNAL(dominantToolChanged(const QString&)), this, SLOT(dominantToolChangedSlot()));
  dominantToolChangedSlot();
}

double DoubleDataAdapterActiveToolOffset::getValue() const
{
  if (mTool)
    return mTool->getTooltipOffset();
  return 0.0;
}

bool DoubleDataAdapterActiveToolOffset::setValue(double val)
{
  if (!mTool)
    return false;
  mTool->setTooltipOffset(val);
  return true;
}

void DoubleDataAdapterActiveToolOffset::dominantToolChangedSlot()
{
  if (mTool)
  {
    disconnect(mTool.get(), SIGNAL(tooltipOffset(double)), this, SIGNAL(changed()));
  }

  mTool = ssc::toolManager()->getDominantTool();

  if (mTool)
  {
    connect(mTool.get(), SIGNAL(tooltipOffset(double)), this, SIGNAL(changed()));
  }

  emit changed();
}

ssc::DoubleRange DoubleDataAdapterActiveToolOffset::getValueRange() const
{
  double range = 200;
  return ssc::DoubleRange(0,range,range/1000.0);
}

//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------


DoubleDataAdapterActiveImageBase::DoubleDataAdapterActiveImageBase()
{
  connect(ssc::dataManager(), SIGNAL(activeImageChanged(const QString&)), this, SLOT(activeImageChanged()));
  connect(ssc::dataManager(), SIGNAL(activeImageTransferFunctionsChanged()), this, SIGNAL(changed()));
}
void DoubleDataAdapterActiveImageBase::activeImageChanged()
{
  mImage = ssc::dataManager()->getActiveImage();
  emit changed();
}
double DoubleDataAdapterActiveImageBase::getValue() const
{
  if (!mImage)
    return 0.0;
  return getValueInternal();
}
bool DoubleDataAdapterActiveImageBase::setValue(double val)
{
  if (!mImage)
    return false;
  setValueInternal(val);
  return true;
}

//---------------------------------------------------------
//---------------------------------------------------------

double DoubleDataAdapter2DWindow::getValueInternal() const
{
  return mImage->getLookupTable2D()->getWindow();
}
void DoubleDataAdapter2DWindow::setValueInternal(double val)
{
  mImage->getLookupTable2D()->setWindow(val);
}
ssc::DoubleRange DoubleDataAdapter2DWindow::getValueRange() const
{
  if (!mImage)
    return ssc::DoubleRange();
  double range = mImage->getRange();
  return ssc::DoubleRange(1,range,range/1000.0);
}




//---------------------------------------------------------
//---------------------------------------------------------




double DoubleDataAdapter2DLevel::getValueInternal() const
{
  return mImage->getLookupTable2D()->getLevel();
}
void DoubleDataAdapter2DLevel::setValueInternal(double val)
{
  mImage->getLookupTable2D()->setLevel(val);
}
ssc::DoubleRange DoubleDataAdapter2DLevel::getValueRange() const
{
  if (!mImage)
    return ssc::DoubleRange();

  double max = mImage->getMax();
  return ssc::DoubleRange(1,max,max/1000.0);
}




//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------

SelectImageStringDataAdapterBase::SelectImageStringDataAdapterBase()
{
  connect(ssc::dataManager(), SIGNAL(dataLoaded()),                         this, SIGNAL(changed()));
  connect(ssc::dataManager(), SIGNAL(currentImageDeleted(ssc::ImagePtr)),   this, SIGNAL(changed()));
}
QStringList SelectImageStringDataAdapterBase::getValueRange() const
{
  std::vector<QString> uids = ssc::dataManager()->getImageUids();
  QStringList retval;
  retval << "";
  for (unsigned i=0; i<uids.size(); ++i)
    retval << qstring_cast(uids[i]);
  return retval;
}
QString SelectImageStringDataAdapterBase::convertInternal2Display(QString internal)
{
  ssc::ImagePtr image = ssc::dataManager()->getImage(internal);
  if (!image)
    return "<no volume>";
  return qstring_cast(image->getName());
}
//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------

SelectDataStringDataAdapterBase::SelectDataStringDataAdapterBase()
{
  connect(ssc::dataManager(), SIGNAL(dataLoaded()),                         this, SIGNAL(changed()));
  connect(ssc::dataManager(), SIGNAL(currentImageDeleted(ssc::ImagePtr)),   this, SIGNAL(changed()));
}
QStringList SelectDataStringDataAdapterBase::getValueRange() const
{
  std::vector<QString> meshUids = ssc::dataManager()->getMeshUids();
  std::vector<QString> imageUids = ssc::dataManager()->getImageUids();
  QStringList retval;
  retval << "";
  for (unsigned i=0; i<meshUids.size(); ++i)
    retval << qstring_cast(meshUids[i]);
  for (unsigned i=0; i<imageUids.size(); ++i)
    retval << qstring_cast(imageUids[i]);
  return retval;
}
QString SelectDataStringDataAdapterBase::convertInternal2Display(QString internal)
{
  ssc::DataPtr data = ssc::dataManager()->getData(internal);
  if (!data)
    return "<no data>";
  return qstring_cast(data->getName());
}
//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------

SelectToolStringDataAdapterBase::SelectToolStringDataAdapterBase()
{
  connect(ssc::toolManager(), SIGNAL(configured()), this, SIGNAL(changed()));
}
QStringList SelectToolStringDataAdapterBase::getValueRange() const
{
  std::vector<QString> uids = ssc::toolManager()->getToolUids();
  QStringList retval;
  retval << "";
  for (unsigned i=0; i<uids.size(); ++i)
    retval << qstring_cast(uids[i]);
  return retval;
}
QString SelectToolStringDataAdapterBase::convertInternal2Display(QString internal)
{
  ssc::ToolPtr tool = ssc::toolManager()->getTool(internal);
  if (!tool)
  {
    return "<no tool>";
  }
  return qstring_cast(tool->getName());
}

//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------
//
//SelectRecordSessionStringDataAdapterBase::SelectRecordSessionStringDataAdapterBase()
//{
//  connect(stateManager(), SIGNAL(recordedSessionsChanged()), this, SIGNAL(changed()));
//}
//QStringList SelectRecordSessionStringDataAdapterBase::getValueRange() const
//{
//  std::vector<RecordSessionPtr> sessions =  stateManager()->getRecordSessions();
//  QStringList retval;
//  retval << "";
//  for (unsigned i=0; i<sessions.size(); ++i)
//    retval << qstring_cast(sessions[i]->getUid());
//  return retval;
//}
//QString SelectRecordSessionStringDataAdapterBase::convertInternal2Display(QString internal)
//{
//  RecordSessionPtr session = stateManager()->getRecordSession(internal);
//  if(!session)
//  {
//    return "<no session>";
//  }
//  return qstring_cast(session->getDescription());
//}

//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------

SelectCoordinateSystemStringDataAdapterBase::SelectCoordinateSystemStringDataAdapterBase()
{
}
QStringList SelectCoordinateSystemStringDataAdapterBase::getValueRange() const
{
  QStringList retval;
  retval << "";
  retval << qstring_cast(ssc::csREF);
  retval << qstring_cast(ssc::csDATA);
  retval << qstring_cast(ssc::csPATIENTREF);
  retval << qstring_cast(ssc::csTOOL);
  retval << qstring_cast(ssc::csSENSOR);
  return retval;
}
QString SelectCoordinateSystemStringDataAdapterBase::convertInternal2Display(QString internal)
{
  if (internal.isEmpty())
    return "<no coordinate system>";

  //as requested by Frank
  if(internal == "reference")
    return "data reference";
  if(internal == "data")
    return "data (image/mesh)";
  if(internal == "patient reference")
    return "patient/tool reference";
  if(internal == "tool")
    return "tool";
  if(internal == "sensor")
    return "tools sensor";

  return internal;
}

//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------

ActiveImageStringDataAdapter::ActiveImageStringDataAdapter()
{
  connect(ssc::dataManager(), SIGNAL(activeImageChanged(QString)),      this, SIGNAL(changed()));
}
QString ActiveImageStringDataAdapter::getValueName() const
{
  return "Active Volume";
}
bool ActiveImageStringDataAdapter::setValue(const QString& value)
{
  ssc::ImagePtr newImage = ssc::dataManager()->getImage(value);
  if (newImage==ssc::dataManager()->getActiveImage())
    return false;
  ssc::dataManager()->setActiveImage(newImage);
  return true;
}
QString ActiveImageStringDataAdapter::getValue() const
{
  if (!ssc::dataManager()->getActiveImage())
    return "";
  return qstring_cast(ssc::dataManager()->getActiveImage()->getUid());
}
QString ActiveImageStringDataAdapter::getHelp() const
{
  return "Select the active volume";
}

//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------

RegistrationFixedImageStringDataAdapter::RegistrationFixedImageStringDataAdapter()
{
  connect(registrationManager(), SIGNAL(fixedDataChanged(QString)), this, SIGNAL(changed()));
}
QString RegistrationFixedImageStringDataAdapter::getValueName() const
{
  return "Fixed Volume";
}

bool RegistrationFixedImageStringDataAdapter::setValue(const QString& value)
{
  ssc::ImagePtr newImage = ssc::dataManager()->getImage(value);
  if (newImage==registrationManager()->getFixedData())
    return false;
  registrationManager()->setFixedData(newImage);
  return true;
}
QString RegistrationFixedImageStringDataAdapter::getValue() const
{
  ssc::ImagePtr image = boost::dynamic_pointer_cast<ssc::Image>(registrationManager()->getFixedData());
  if (!image)
    return "";
  return qstring_cast(image->getUid());
}
QString RegistrationFixedImageStringDataAdapter::getHelp() const
{
  return "Select the fixed registration volume";
}

//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------

RegistrationMovingImageStringDataAdapter::RegistrationMovingImageStringDataAdapter()
{
  connect(registrationManager(), SIGNAL(movingDataChanged(QString)), this, SIGNAL(changed()));
}
QString RegistrationMovingImageStringDataAdapter::getValueName() const
{
  return "Moving Volume";
}
bool RegistrationMovingImageStringDataAdapter::setValue(const QString& value)
{
  ssc::ImagePtr newImage = ssc::dataManager()->getImage(value);
  if (newImage==registrationManager()->getMovingData())
    return false;
  registrationManager()->setMovingData(newImage);
  return true;
}
QString RegistrationMovingImageStringDataAdapter::getValue() const
{
  ssc::ImagePtr image = boost::dynamic_pointer_cast<ssc::Image>(registrationManager()->getMovingData());
  if (!image)
    return "";
  return qstring_cast(image->getUid());
}
QString RegistrationMovingImageStringDataAdapter::getHelp() const
{
  return "Select the moving registration volume";
}


//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------

SelectImageStringDataAdapter::SelectImageStringDataAdapter() :
    mValueName("Select volume")
{
}
QString SelectImageStringDataAdapter::getValueName() const
{
  return mValueName;
}
bool SelectImageStringDataAdapter::setValue(const QString& value)
{
  if (value==mImageUid)
    return false;
  mImageUid = value;
  emit changed();
  emit imageChanged(mImageUid);
  return true;
}
QString SelectImageStringDataAdapter::getValue() const
{
  return mImageUid;
}
QString SelectImageStringDataAdapter::getHelp() const
{
  return "Select a volume";
}
ssc::ImagePtr SelectImageStringDataAdapter::getImage()
{
  return ssc::dataManager()->getImage(mImageUid);
}

void SelectImageStringDataAdapter::setValueName(const QString name)
{
  mValueName = name;
}
//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------

SelectCoordinateSystemStringDataAdapter::SelectCoordinateSystemStringDataAdapter()
{
  connect(ssc::toolManager(), SIGNAL(configured()), this, SLOT(setDefaultSlot()));
}
QString SelectCoordinateSystemStringDataAdapter::getValueName() const
{
  return "Select coordinate system";
}
bool SelectCoordinateSystemStringDataAdapter::setValue(const QString& value)
{
  mCoordinateSystem = string2enum<ssc::COORDINATE_SYSTEM>(value);
  emit changed();
  return true;
}
QString SelectCoordinateSystemStringDataAdapter::getValue() const
{
  return qstring_cast(mCoordinateSystem);
}
QString SelectCoordinateSystemStringDataAdapter::getHelp() const
{
  return "Select a coordinate system";
}
void SelectCoordinateSystemStringDataAdapter::setDefaultSlot()
{
  this->setValue(qstring_cast(ssc::csPATIENTREF));
}
//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------

SelectToolStringDataAdapter::SelectToolStringDataAdapter()
{
}
QString SelectToolStringDataAdapter::getValueName() const
{
  return "Select a tool";
}
bool SelectToolStringDataAdapter::setValue(const QString& value)
{
  if(mTool && value==mTool->getUid())
    return false;
  ssc::ToolPtr temp = ssc::toolManager()->getTool(value);
  if(!temp)
    return false;

  mTool = temp;
  emit changed();
  return true;
}
QString SelectToolStringDataAdapter::getValue() const
{
  if(!mTool)
    return "<no tool>";
  return mTool->getUid();
}
QString SelectToolStringDataAdapter::getHelp() const
{
  return "Select a tool";
}
ssc::ToolPtr SelectToolStringDataAdapter::getTool() const
{
  return mTool;
}

//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------
//
//SelectRecordSessionStringDataAdapter::SelectRecordSessionStringDataAdapter()
//{
//}
//QString SelectRecordSessionStringDataAdapter::getValueName() const
//{
//  return "Select a record session";
//}
//bool SelectRecordSessionStringDataAdapter::setValue(const QString& value)
//{
//  if(mRecordSession && value==mRecordSession->getUid())
//    return false;
//  RecordSessionPtr temp = stateManager()->getRecordSession(value);
//  if(!temp)
//    return false;
//
//  mRecordSession = temp;
//  emit changed();
//  return true;
//}
//QString SelectRecordSessionStringDataAdapter::getValue() const
//{
//  if(!mRecordSession)
//    return "<no session>";
//  return mRecordSession->getUid();
//}
//QString SelectRecordSessionStringDataAdapter::getHelp() const
//{
//  return "Select a session";
//}
//RecordSessionPtr SelectRecordSessionStringDataAdapter::getRecordSession()
//{
//  return mRecordSession;
//}

//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------

SelectDataStringDataAdapter::SelectDataStringDataAdapter()
{
}
QString SelectDataStringDataAdapter::getValueName() const
{
  return "Select data";
}
bool SelectDataStringDataAdapter::setValue(const QString& value)
{
  if (mData && value==mData->getUid())
    return false;
  ssc::DataPtr temp = ssc::dataManager()->getData(value);
  if(!temp)
    return false;

  mData = temp;
  emit changed();
  return true;
}
QString SelectDataStringDataAdapter::getValue() const
{
  if(!mData)
    return "<no data>";
  return mData->getUid();
}
QString SelectDataStringDataAdapter::getHelp() const
{
  return "Select data";
}
ssc::DataPtr SelectDataStringDataAdapter::getData() const
{
  return mData;
}
//---------------------------------------------------------

SelectMeshStringDataAdapter::SelectMeshStringDataAdapter() :
    mValueName("Select mesh")
{
  connect(ssc::dataManager(), SIGNAL(dataLoaded()),                         this, SIGNAL(changed()));
  connect(ssc::dataManager(), SIGNAL(currentImageDeleted(ssc::ImagePtr)),   this, SIGNAL(changed()));
}
QString SelectMeshStringDataAdapter::getValueName() const
{
  return mValueName;
}
bool SelectMeshStringDataAdapter::setValue(const QString& value)
{
  if (value==mMeshUid)
    return false;
  mMeshUid = value;
  emit changed();
  emit meshChanged(mMeshUid);
  return true;
}
QString SelectMeshStringDataAdapter::getValue() const
{
  return mMeshUid;
}
QString SelectMeshStringDataAdapter::getHelp() const
{
  return "Select a mesh";
}
QStringList SelectMeshStringDataAdapter::getValueRange() const
{
  std::vector<QString> uids = ssc::dataManager()->getMeshUids();
  QStringList retval;
  retval << "";
  for (unsigned i=0; i<uids.size(); ++i)
    retval << qstring_cast(uids[i]);
  return retval;
}
QString SelectMeshStringDataAdapter::convertInternal2Display(QString internal)
{
  ssc::MeshPtr mesh = ssc::dataManager()->getMesh(internal);
  if (!mesh)
    return "<no mesh>";
  return qstring_cast(mesh->getName());
}
ssc::MeshPtr SelectMeshStringDataAdapter::getMesh()
{
  return ssc::dataManager()->getMesh(mMeshUid);
}

void SelectMeshStringDataAdapter::setValueName(const QString name)
{
  mValueName = name;
}

//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------




ParentFrameStringDataAdapter::ParentFrameStringDataAdapter()
{
  connect(ssc::dataManager(), SIGNAL(dataLoaded()), this, SIGNAL(changed()));
}

void ParentFrameStringDataAdapter::setData(ssc::DataPtr data)
{
  if (mData)
    disconnect(mData.get(), SIGNAL(transformChanged()), this, SIGNAL(changed()));
  mData = data;
  if (mData)
    connect(mData.get(), SIGNAL(transformChanged()), this, SIGNAL(changed()));
  emit changed();
}

QString ParentFrameStringDataAdapter::getValueName() const
{
  return "Parent Frame";
}

bool ParentFrameStringDataAdapter::setValue(const QString& value)
{
  if (!mData)
    return false;
  mData->get_rMd_History()->addParentFrame(value);
  return true;
}

QString ParentFrameStringDataAdapter::getValue() const
{
  if (!mData)
    return "";
  return qstring_cast(mData->getParentFrame());
}

QString ParentFrameStringDataAdapter::getHelp() const
{
  if (!mData)
    return "";
  return "Select the parent frame for " + qstring_cast(mData->getName()) + ".";
}

QStringList ParentFrameStringDataAdapter::getValueRange() const
{
  QStringList retval;
  retval << "";

  std::map<QString, ssc::DataPtr> allData = ssc::dataManager()->getData();
  for (std::map<QString, ssc::DataPtr>::iterator iter=allData.begin(); iter!=allData.end(); ++iter)
  {
    if (mData && (mData->getUid() == iter->first))
      continue;

    retval << qstring_cast(iter->first);
  }
  return retval;
}

QString ParentFrameStringDataAdapter::convertInternal2Display(QString internal)
{
  ssc::DataPtr data = ssc::dataManager()->getData(internal);
  if (!data)
    return "<no data>";
  return qstring_cast(data->getName());
}

//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------

DataNameEditableStringDataAdapter::DataNameEditableStringDataAdapter()
{
}

QString DataNameEditableStringDataAdapter::getValueName() const
{
  return "Name";
}

bool DataNameEditableStringDataAdapter::setValue(const QString& value)
{
  if (!mData)
    return false;
  mData->setName(value);
  return true;
}

QString DataNameEditableStringDataAdapter::getValue() const
{
  if (mData)
    return mData->getName();
  return "";
}

void DataNameEditableStringDataAdapter::setData(ssc::DataPtr data)
{
  mData = data;
  emit changed();
}

//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------

DataUidEditableStringDataAdapter::DataUidEditableStringDataAdapter()
{
}

QString DataUidEditableStringDataAdapter::getValueName() const
{
  return "Uid";
}

bool DataUidEditableStringDataAdapter::setValue(const QString& value)
{
  return false;
}

QString DataUidEditableStringDataAdapter::getValue() const
{
  if (mData)
    return mData->getUid();
  return "";
}

void DataUidEditableStringDataAdapter::setData(ssc::DataPtr data)
{
  mData = data;
  emit changed();
}


} // namespace cx
