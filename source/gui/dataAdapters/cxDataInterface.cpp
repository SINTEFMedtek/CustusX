// This file is part of CustusX, an Image Guided Therapy Application.
//
// Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
//
// CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
// code and binaries can only be used by SMT and those with explicit permission
// from SMT. CustusX shall not be distributed to anyone else.
//
// CustusX is a research tool. It is NOT intended for use or certified for use
// in a normal clinical setting. SMT does not take responsibility for its use
// in any way.
//
// See CustusX_License.txt for more information.
#include "cxDataInterface.h"
#include "cxImage.h"
#include <QSet>
#include "cxMesh.h"
#include "cxReporter.h"
#include "cxImageLUT2D.h"
#include "cxDataManager.h"
#include "cxToolManager.h"
#include "cxTypeConversions.h"
#include "cxDefinitionStrings.h"
#include "cxEnumConverter.h"
#include "cxTool.h"
#include "cxImageAlgorithms.h"
#include "cxRegistrationTransform.h"
#include "cxLogger.h"
#include "cxActiveImageProxy.h"
#include "cxVideoSource.h"
#include "cxVideoService.h"

namespace cx
{
DoubleDataAdapterActiveToolOffset::DoubleDataAdapterActiveToolOffset()
{
  connect(toolManager(), SIGNAL(dominantToolChanged(const QString&)), this, SLOT(dominantToolChangedSlot()));
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

  mTool = toolManager()->getDominantTool();

  if (mTool)
  {
    connect(mTool.get(), SIGNAL(tooltipOffset(double)), this, SIGNAL(changed()));
  }

  emit changed();
}

DoubleRange DoubleDataAdapterActiveToolOffset::getValueRange() const
{
  double range = 200;
  return DoubleRange(0,range,1);
}

//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------

DoubleDataAdapterActiveImageBase::DoubleDataAdapterActiveImageBase()
{
	mActiveImageProxy = ActiveImageProxy::New(dataService());
  connect(mActiveImageProxy.get(), SIGNAL(activeImageChanged(QString)), this, SLOT(activeImageChanged()));
  connect(mActiveImageProxy.get(), SIGNAL(transferFunctionsChanged()), this, SIGNAL(changed()));
}
void DoubleDataAdapterActiveImageBase::activeImageChanged()
{
  mImage = dataManager()->getActiveImage();
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
DoubleRange DoubleDataAdapter2DWindow::getValueRange() const
{
  if (!mImage)
    return DoubleRange();
  double range = mImage->getRange();
  return DoubleRange(1,range,range/1000.0);
}

//---------------------------------------------------------
//---------------------------------------------------------

double DoubleDataAdapter2DLevel::getValueInternal() const
{
  return mImage->getLookupTable2D()->getLevel();
}
void DoubleDataAdapter2DLevel::setValueInternal(double val)
{
	SSC_LOG("");
  mImage->getLookupTable2D()->setLevel(val);
}
DoubleRange DoubleDataAdapter2DLevel::getValueRange() const
{
  if (!mImage)
    return DoubleRange();

  double max = mImage->getMax();
  return DoubleRange(1,max,max/1000.0);
}

////---------------------------------------------------------
////---------------------------------------------------------
////---------------------------------------------------------

SelectRTSourceStringDataAdapterBase::SelectRTSourceStringDataAdapterBase()
{
  connect(dataManager(), SIGNAL(streamLoaded()), this, SIGNAL(changed()));
}

QStringList SelectRTSourceStringDataAdapterBase::getValueRange() const
{
  DataManager::StreamMap streams = dataManager()->getStreams();
  QStringList retval;
  retval << "<no real time source>";
  DataManager::StreamMap::iterator it = streams.begin();
  for (; it !=streams.end(); ++it)
    retval << qstring_cast(it->second->getUid());
  return retval;
}

QString SelectRTSourceStringDataAdapterBase::convertInternal2Display(QString internal)
{
  VideoSourcePtr rtSource = dataManager()->getStream(internal);
  if (!rtSource)
    return "<no real time source>";
  return qstring_cast(rtSource->getName());
}
//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------

ActiveVideoSourceStringDataAdapter::ActiveVideoSourceStringDataAdapter()
{
	connect(videoService().get(), SIGNAL(activeVideoSourceChanged()), this, SIGNAL(changed()));
}

QString ActiveVideoSourceStringDataAdapter::getValueName() const
{
	return "VideoSource";
}

bool ActiveVideoSourceStringDataAdapter::setValue(const QString& value)
{
	if (value == this->getValue())
		return false;
	videoService()->setActiveVideoSource(value);
	emit changed();
	return true;
}

QString ActiveVideoSourceStringDataAdapter::getValue() const
{
	return videoService()->getActiveVideoSource()->getUid();
}

QStringList ActiveVideoSourceStringDataAdapter::getValueRange() const
{
	std::vector<VideoSourcePtr> sources = videoService()->getVideoSources();
	QStringList retval;
	for (unsigned i=0; i<sources.size(); ++i)
		retval << sources[i]->getUid();
	return retval;
}

QString ActiveVideoSourceStringDataAdapter::getHelp() const
{
	return "Select the active video source.";
}

//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------

SelectToolStringDataAdapterBase::SelectToolStringDataAdapterBase()
{
  connect(toolManager(), SIGNAL(configured()), this, SIGNAL(changed()));
}

QStringList SelectToolStringDataAdapterBase::getValueRange() const
{
  std::vector<QString> uids = toolManager()->getToolUids();
  QStringList retval;
  retval << "";
  for (unsigned i=0; i<uids.size(); ++i)
    retval << qstring_cast(uids[i]);
  return retval;
}

QString SelectToolStringDataAdapterBase::convertInternal2Display(QString internal)
{
  ToolPtr tool = toolManager()->getTool(internal);
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
  retval << qstring_cast(csREF);
  retval << qstring_cast(csDATA);
  retval << qstring_cast(csPATIENTREF);
  retval << qstring_cast(csTOOL);
  retval << qstring_cast(csSENSOR);
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


SelectRTSourceStringDataAdapter::SelectRTSourceStringDataAdapter() :
    mValueName("Select Real Time Source")
{
  connect(dataManager(), SIGNAL(streamLoaded()), this, SLOT(setDefaultSlot()));
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

  VideoSourcePtr rtSource = dataManager()->getStream(value);
  if(!rtSource)
    return false;

  mRTSource = rtSource;
  connect(mRTSource.get(), SIGNAL(streaming(bool)), this, SIGNAL(changed()));

  emit changed();
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

VideoSourcePtr SelectRTSourceStringDataAdapter::getRTSource()
{
  return mRTSource;
}

void SelectRTSourceStringDataAdapter::setValueName(const QString name)
{
  mValueName = name;
}

void SelectRTSourceStringDataAdapter::setDefaultSlot()
{
  DataManager::StreamMap streams = dataManager()->getStreams();
  DataManager::StreamMap::iterator it = streams.begin();
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
	mCoordinateSystem = csCOUNT;
  connect(toolManager(), SIGNAL(configured()), this, SLOT(setDefaultSlot()));
}

QString SelectCoordinateSystemStringDataAdapter::getValueName() const
{
  return "Select coordinate system";
}

bool SelectCoordinateSystemStringDataAdapter::setValue(const QString& value)
{
  mCoordinateSystem = string2enum<COORDINATE_SYSTEM>(value);
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
  this->setValue(qstring_cast(csPATIENTREF));
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
  ToolPtr temp = toolManager()->getTool(value);
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

ToolPtr SelectToolStringDataAdapter::getTool() const
{
  return mTool;
}

//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------


ParentFrameStringDataAdapter::ParentFrameStringDataAdapter()
{
  connect(dataManager(), SIGNAL(dataAddedOrRemoved()), this, SIGNAL(changed()));
}

void ParentFrameStringDataAdapter::setData(DataPtr data)
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

  std::map<QString, DataPtr> allData = dataManager()->getData();
  for (std::map<QString, DataPtr>::iterator iter=allData.begin(); iter!=allData.end(); ++iter)
  {
    if (mData && (mData->getUid() == iter->first))
      continue;

    retval << qstring_cast(iter->first);
  }
  return retval;
}

QString ParentFrameStringDataAdapter::convertInternal2Display(QString internal)
{
  DataPtr data = dataManager()->getData(internal);
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

void DataNameEditableStringDataAdapter::setData(DataPtr data)
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

void DataUidEditableStringDataAdapter::setData(DataPtr data)
{
  mData = data;
  emit changed();
}

//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------

DataModalityStringDataAdapter::DataModalityStringDataAdapter()
{
	connect(dataManager(), SIGNAL(dataAddedOrRemoved()), this, SIGNAL(changed()));
}

void DataModalityStringDataAdapter::setData(ImagePtr data)
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
	connect(dataManager(), SIGNAL(dataAddedOrRemoved()), this, SIGNAL(changed()));
}

void ImageTypeStringDataAdapter::setData(ImagePtr data)
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
