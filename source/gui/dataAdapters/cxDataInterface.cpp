/*
 * cxDataInterface.cpp
 *
 *  \date Apr 13, 2010
 *      \author christiana
 */
#include "cxDataInterface.h"
#include "sscImage.h"
#include <QSet>
#include "sscMesh.h"
#include "sscMessageManager.h"
#include "sscImageLUT2D.h"
#include "sscDataManager.h"
#include "sscToolManager.h"
#include "sscTypeConversions.h"
#include "sscDefinitionStrings.h"
#include "sscEnumConverter.h"
#include "sscTool.h"
#include "sscImageAlgorithms.h"
#include "sscRegistrationTransform.h"

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
  return ssc::DoubleRange(0,range,1);
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
  std::vector<ssc::ImagePtr> sorted = sortOnGroupsAndAcquisitionTime(ssc::dataManager()->getImages());
//  std::vector<QString> uids = ssc::dataManager()->getImageUids();
  QStringList retval;
  retval << "";
//  for (unsigned i=0; i<uids.size(); ++i)
//    retval << qstring_cast(uids[i]);
  for (unsigned i=0; i<sorted.size(); ++i)
    retval << sorted[i]->getUid();
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

SelectRTSourceStringDataAdapterBase::SelectRTSourceStringDataAdapterBase()
{
  connect(ssc::dataManager(), SIGNAL(streamLoaded()), this, SIGNAL(changed()));
}
QStringList SelectRTSourceStringDataAdapterBase::getValueRange() const
{
  ssc::DataManager::StreamMap streams = ssc::dataManager()->getStreams();
  QStringList retval;
  retval << "<no real time source>";
  ssc::DataManager::StreamMap::iterator it = streams.begin();
  for (; it !=streams.end(); ++it)
    retval << qstring_cast(it->second->getUid());
  return retval;
}
QString SelectRTSourceStringDataAdapterBase::convertInternal2Display(QString internal)
{
  ssc::VideoSourcePtr rtSource = ssc::dataManager()->getStream(internal);
  if (!rtSource)
    return "<no real time source>";
  return qstring_cast(rtSource->getName());
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
  std::vector<ssc::DataPtr> sorted = sortOnGroupsAndAcquisitionTime(ssc::dataManager()->getData());
  QStringList retval;
  retval << "";
  for (unsigned i=0; i<sorted.size(); ++i)
    retval << sorted[i]->getUid();
  return retval;

//  std::vector<QString> meshUids = ssc::dataManager()->getMeshUids();
//  std::vector<QString> imageUids = ssc::dataManager()->getImageUids();
//  QStringList retval;
//  retval << "";
//  for (unsigned i=0; i<meshUids.size(); ++i)
//    retval << qstring_cast(meshUids[i]);
//  for (unsigned i=0; i<imageUids.size(); ++i)
//    retval << qstring_cast(imageUids[i]);
//  return retval;
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

//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------

SelectImageStringDataAdapter::SelectImageStringDataAdapter() :
    mValueName("Select volume"),
    mHelp("Select a volume")
{
}
void SelectImageStringDataAdapter::setHelp(QString text)
{
	mHelp = text;
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
  return mHelp;
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

SelectRTSourceStringDataAdapter::SelectRTSourceStringDataAdapter() :
    mValueName("Select Real Time Source")
{
  connect(ssc::dataManager(), SIGNAL(streamLoaded()), this, SLOT(setDefaultSlot()));
  this->setDefaultSlot();
}
QString SelectRTSourceStringDataAdapter::getValueName() const
{
  return mValueName;
}
bool SelectRTSourceStringDataAdapter::setValue(const QString& value)
{
  if(mRTSource && (mRTSource->getUid() == value))
    return false;

  if(mRTSource)
    disconnect(mRTSource.get(), SIGNAL(streaming(bool)), this, SIGNAL(changed()));

  ssc::VideoSourcePtr rtSource = ssc::dataManager()->getStream(value);
  if(!rtSource)
    return false;

  mRTSource = rtSource;
  connect(mRTSource.get(), SIGNAL(streaming(bool)), this, SIGNAL(changed()));

  emit changed();
  //emit rtSourceChanged();
  return true;
}
QString SelectRTSourceStringDataAdapter::getValue() const
{
  if(!mRTSource)
    return "<no real time source>";
  return mRTSource->getUid();
}
QString SelectRTSourceStringDataAdapter::getHelp() const
{
  return "Select a real time source";
}
ssc::VideoSourcePtr SelectRTSourceStringDataAdapter::getRTSource()
{
  return mRTSource;
}

void SelectRTSourceStringDataAdapter::setValueName(const QString name)
{
  mValueName = name;
}
void SelectRTSourceStringDataAdapter::setDefaultSlot()
{
  ssc::DataManager::StreamMap streams = ssc::dataManager()->getStreams();
  ssc::DataManager::StreamMap::iterator it = streams.begin();
  if(it != streams.end())
  {
    this->setValue(it->first);
  }
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
	mValueName = "Select a tool";
	mHelp = mValueName;
}
void SelectToolStringDataAdapter::setHelp(QString help)
{
  mHelp = help;
}
void SelectToolStringDataAdapter::setValueName(QString name)
{
  mValueName = name;
}
QString SelectToolStringDataAdapter::getValueName() const
{
  return mValueName;
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
  return mHelp;
}
ssc::ToolPtr SelectToolStringDataAdapter::getTool() const
{
  return mTool;
}


//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------

SelectDataStringDataAdapter::SelectDataStringDataAdapter() :
    mValueName("Select data")
{
}
void SelectDataStringDataAdapter::setValueName(const QString name)
{
  mValueName = name;
}
QString SelectDataStringDataAdapter::getValueName() const
{
  return mValueName;
}
bool SelectDataStringDataAdapter::setValue(const QString& value)
{
  if (mData && value==mData->getUid())
    return false;
  ssc::DataPtr temp = ssc::dataManager()->getData(value);
//  if(!temp) // wrong: must accept to set to zero
//    return false;

  mData = temp;
  emit changed();
//  emit dataChanged(value);
  emit dataChanged(this->getValue());
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
  std::vector<ssc::MeshPtr> sorted = sortOnGroupsAndAcquisitionTime(ssc::dataManager()->getMeshes());
  QStringList retval;
  retval << "";
  for (unsigned i=0; i<sorted.size(); ++i)
    retval << sorted[i]->getUid();
  return retval;

//  std::vector<QString> uids = ssc::dataManager()->getMeshUids();
//  QStringList retval;
//  retval << "";
//  for (unsigned i=0; i<uids.size(); ++i)
//    retval << qstring_cast(uids[i]);
//  return retval;
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
  mData->get_rMd_History()->addParentSpace(value);
  return true;
}

QString ParentFrameStringDataAdapter::getValue() const
{
  if (!mData)
    return "";
  return qstring_cast(mData->getParentSpace());
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


bool SetParentFrameStringDataAdapter::setValue(const QString& value)
{
  if (!mData)
    return false;
  mData->get_rMd_History()->setParentSpace(value);
  return true;
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


//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------


DataModalityStringDataAdapter::DataModalityStringDataAdapter()
{
	connect(ssc::dataManager(), SIGNAL(dataLoaded()), this, SIGNAL(changed()));
}

void DataModalityStringDataAdapter::setData(ssc::ImagePtr data)
{
	if (mData)
		disconnect(mData.get(), SIGNAL(propertiesChanged()), this, SIGNAL(changed()));
	mData = data;
	if (mData)
		connect(mData.get(), SIGNAL(propertiesChanged()), this, SIGNAL(changed()));
	emit changed();
}

QString DataModalityStringDataAdapter::getValueName() const
{
	return "Modality";
}

bool DataModalityStringDataAdapter::setValue(const QString& value)
{
	if (!mData)
		return false;
	mData->setModality(value);
	return true;
}

QString DataModalityStringDataAdapter::getValue() const
{
	if (!mData)
		return "";
	return mData->getModality();
}

QString DataModalityStringDataAdapter::getHelp() const
{
	if (!mData)
		return "";
	return "Select the modality for " + qstring_cast(mData->getName()) + ".";
}

QStringList DataModalityStringDataAdapter::getValueRange() const
{
	QStringList retval;
	retval << "";
	if (mData)
		retval << mData->getModality();
	retval << "CT" << "MR" << "US";
	return QStringList::fromSet(QSet<QString>::fromList(retval));
}


//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------




ImageTypeStringDataAdapter::ImageTypeStringDataAdapter()
{
	connect(ssc::dataManager(), SIGNAL(dataLoaded()), this, SIGNAL(changed()));
}

void ImageTypeStringDataAdapter::setData(ssc::ImagePtr data)
{
	if (mData)
		disconnect(mData.get(), SIGNAL(propertiesChanged()), this, SIGNAL(changed()));
	mData = data;
	if (mData)
		connect(mData.get(), SIGNAL(propertiesChanged()), this, SIGNAL(changed()));
	emit changed();
}

QString ImageTypeStringDataAdapter::getValueName() const
{
	return "Image Type";
}

bool ImageTypeStringDataAdapter::setValue(const QString& value)
{
	if (!mData)
		return false;
	mData->setImageType(value);
	return true;
}

QString ImageTypeStringDataAdapter::getValue() const
{
	if (!mData)
		return "";
	return mData->getImageType();
}

QString ImageTypeStringDataAdapter::getHelp() const
{
	if (!mData)
		return "";
	return "Select the image type for " + qstring_cast(mData->getName()) + ".";
}

QStringList ImageTypeStringDataAdapter::getValueRange() const
{
	QStringList retval;
	retval << "";
	if (mData)
	{
		retval << mData->getImageType();
		if (mData->getModality()=="CT")
			retval << "";
		if (mData->getModality()=="MR")
			retval << "T1" << "T2" << "ANGIO";
		if (mData->getModality()=="US")
			retval << "B-Mode" << "Angio";
	}
	return QStringList::fromSet(QSet<QString>::fromList(retval));
}

} // namespace cx
