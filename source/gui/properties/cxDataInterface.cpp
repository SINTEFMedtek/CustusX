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
#include "cxDataInterface.h"
#include "cxImage.h"
#include <QSet>
#include "cxMesh.h"
#include "cxImageLUT2D.h"
#include "cxTrackingService.h"
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
#include "cxPatientModelService.h"
#include "cxActiveToolProxy.h"

//TODO: remove
#include "cxLegacySingletons.h"

namespace cx
{
DoublePropertyActiveToolOffset::DoublePropertyActiveToolOffset()
{
  mActiveTool = ActiveToolProxy::New(trackingService());
  connect(mActiveTool.get(), &ActiveToolProxy::tooltipOffset, this, &Property::changed);
}

double DoublePropertyActiveToolOffset::getValue() const
{
	return mActiveTool->getTool()->getTooltipOffset();
}

bool DoublePropertyActiveToolOffset::setValue(double val)
{
  mActiveTool->getTool()->setTooltipOffset(val);
  return true;
}

DoubleRange DoublePropertyActiveToolOffset::getValueRange() const
{
  double range = 200;
  return DoubleRange(0,range,1);
}

//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------

DoublePropertyActiveImageBase::DoublePropertyActiveImageBase(PatientModelServicePtr patientModelService) :
	mPatientModelService(patientModelService)
{
	mActiveImageProxy = ActiveImageProxy::New(patientModelService);
	connect(mActiveImageProxy.get(), &ActiveImageProxy::activeImageChanged, this, &DoublePropertyActiveImageBase::activeImageChanged);
	connect(mActiveImageProxy.get(), &ActiveImageProxy::transferFunctionsChanged, this, &Property::changed);
}
void DoublePropertyActiveImageBase::activeImageChanged()
{
  mImage = mPatientModelService->getActiveData<Image>();
  emit changed();
}
double DoublePropertyActiveImageBase::getValue() const
{
  if (!mImage)
    return 0.0;
  return getValueInternal();
}
bool DoublePropertyActiveImageBase::setValue(double val)
{
  if (!mImage)
    return false;
  setValueInternal(val);
  return true;
}

//---------------------------------------------------------
//---------------------------------------------------------

double DoubleProperty2DWindow::getValueInternal() const
{
  return mImage->getLookupTable2D()->getWindow();
}
void DoubleProperty2DWindow::setValueInternal(double val)
{
  mImage->getLookupTable2D()->setWindow(val);
}
DoubleRange DoubleProperty2DWindow::getValueRange() const
{
  if (!mImage)
    return DoubleRange();
  double range = mImage->getRange();
  return DoubleRange(1,range,range/1000.0);
}

//---------------------------------------------------------
//---------------------------------------------------------

double DoubleProperty2DLevel::getValueInternal() const
{
  return mImage->getLookupTable2D()->getLevel();
}
void DoubleProperty2DLevel::setValueInternal(double val)
{
  mImage->getLookupTable2D()->setLevel(val);
}
DoubleRange DoubleProperty2DLevel::getValueRange() const
{
  if (!mImage)
    return DoubleRange();

  double max = mImage->getMax();
  return DoubleRange(1,max,max/1000.0);
}

////---------------------------------------------------------
////---------------------------------------------------------
////---------------------------------------------------------

StringPropertySelectRTSourceBase::StringPropertySelectRTSourceBase(PatientModelServicePtr patientModelService) :
	mPatientModelService(patientModelService)
{
	connect(mPatientModelService.get(), &PatientModelService::streamLoaded, this, &Property::changed);
}

StringPropertySelectRTSourceBase::~StringPropertySelectRTSourceBase()
{
	disconnect(mPatientModelService.get(), &PatientModelService::streamLoaded, this, &Property::changed);
}

QStringList StringPropertySelectRTSourceBase::getValueRange() const
{
  std::map<QString, VideoSourcePtr> streams = mPatientModelService->getStreams();
  QStringList retval;
  retval << "<no real time source>";
  std::map<QString, VideoSourcePtr>::iterator it = streams.begin();
  for (; it !=streams.end(); ++it)
    retval << qstring_cast(it->second->getUid());
  return retval;
}

QString StringPropertySelectRTSourceBase::convertInternal2Display(QString internal)
{
  VideoSourcePtr rtSource = mPatientModelService->getStream(internal);
  if (!rtSource)
    return "<no real time source>";
  return qstring_cast(rtSource->getName());
}
//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------

StringPropertyActiveVideoSource::StringPropertyActiveVideoSource()
{
	connect(videoService().get(), &VideoService::activeVideoSourceChanged, this, &Property::changed);
}

QString StringPropertyActiveVideoSource::getDisplayName() const
{
	return "Stream";
}

bool StringPropertyActiveVideoSource::setValue(const QString& value)
{
	if (value == this->getValue())
		return false;
	videoService()->setActiveVideoSource(value);
	emit changed();
	return true;
}

QString StringPropertyActiveVideoSource::getValue() const
{
	return videoService()->getActiveVideoSource()->getUid();
}

QStringList StringPropertyActiveVideoSource::getValueRange() const
{
	std::vector<VideoSourcePtr> sources = videoService()->getVideoSources();
	QStringList retval;
	for (unsigned i=0; i<sources.size(); ++i)
		retval << sources[i]->getUid();
	return retval;
}

QString StringPropertyActiveVideoSource::getHelp() const
{
	return "Select the active video source.";
}


//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------


StringPropertySelectRTSource::StringPropertySelectRTSource(PatientModelServicePtr patientModelService) :
	StringPropertySelectRTSourceBase(patientModelService),
    mValueName("Select Real Time Source")
{
	connect(patientModelService.get(), &PatientModelService::streamLoaded, this, &StringPropertySelectRTSource::setDefaultSlot);
	this->setDefaultSlot();
}

QString StringPropertySelectRTSource::getDisplayName() const
{
  return mValueName;
}

bool StringPropertySelectRTSource::setValue(const QString& value)
{
  if(mRTSource && (mRTSource->getUid() == value))
    return false;

  if(mRTSource)
	  disconnect(mRTSource.get(), &VideoSource::streaming, this, &Property::changed);

  VideoSourcePtr rtSource = mPatientModelService->getStream(value);
  if(!rtSource)
    return false;

  mRTSource = rtSource;
  connect(mRTSource.get(), &VideoSource::streaming, this, &Property::changed);

  emit changed();
  return true;
}

QString StringPropertySelectRTSource::getValue() const
{
  if(!mRTSource)
    return "<no real time source>";
  return mRTSource->getUid();
}

QString StringPropertySelectRTSource::getHelp() const
{
  return "Select a real time source";
}

VideoSourcePtr StringPropertySelectRTSource::getRTSource()
{
  return mRTSource;
}

void StringPropertySelectRTSource::setValueName(const QString name)
{
  mValueName = name;
}

void StringPropertySelectRTSource::setDefaultSlot()
{
  std::map<QString, VideoSourcePtr> streams = mPatientModelService->getStreams();
  std::map<QString, VideoSourcePtr>::iterator it = streams.begin();
  if(it != streams.end())
  {
    this->setValue(it->first);
  }
}


//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------


StringPropertyParentFrame::StringPropertyParentFrame(PatientModelServicePtr patientModelService) :
	mPatientModelService(patientModelService)
{
	connect(mPatientModelService.get(), &PatientModelService::dataAddedOrRemoved, this, &Property::changed);
}

void StringPropertyParentFrame::setData(DataPtr data)
{
  if (mData)
	  disconnect(mData.get(), &Data::transformChanged, this, &Property::changed);
  mData = data;
  if (mData)
	  connect(mData.get(), &Data::transformChanged, this, &Property::changed);
  emit changed();
}

StringPropertyParentFrame::~StringPropertyParentFrame()
{
	disconnect(mPatientModelService.get(), &PatientModelService::dataAddedOrRemoved, this, &Property::changed);
}

QString StringPropertyParentFrame::getDisplayName() const
{
  return "Parent Frame";
}

bool StringPropertyParentFrame::setValue(const QString& value)
{
  if (!mData)
    return false;
  mData->get_rMd_History()->addParentSpace(value);
  return true;
}

QString StringPropertyParentFrame::getValue() const
{
  if (!mData)
    return "";
  return qstring_cast(mData->getParentSpace());
}

QString StringPropertyParentFrame::getHelp() const
{
  if (!mData)
    return "";
  return "Select the parent frame for " + qstring_cast(mData->getName()) + ".";
}

QStringList StringPropertyParentFrame::getValueRange() const
{
  QStringList retval;
  retval << "";

  std::map<QString, DataPtr> allData = mPatientModelService->getData();
  for (std::map<QString, DataPtr>::iterator iter=allData.begin(); iter!=allData.end(); ++iter)
  {
    if (mData && (mData->getUid() == iter->first))
      continue;

    retval << qstring_cast(iter->first);
  }
  return retval;
}

QString StringPropertyParentFrame::convertInternal2Display(QString internal)
{
  DataPtr data = mPatientModelService->getData(internal);
  if (!data)
    return "<no data>";
  return qstring_cast(data->getName());
}

//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------

StringPropertySetParentFrame::StringPropertySetParentFrame(PatientModelServicePtr patientModelService) :
	StringPropertyParentFrame(patientModelService)
{
}

bool StringPropertySetParentFrame::setValue(const QString& value)
{
  if (!mData)
    return false;
  mData->get_rMd_History()->setParentSpace(value);
  return true;
}

//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------

StringPropertyDataNameEditable::StringPropertyDataNameEditable()
{
}

QString StringPropertyDataNameEditable::getDisplayName() const
{
  return "Name";
}

bool StringPropertyDataNameEditable::setValue(const QString& value)
{
  if (!mData)
    return false;
  mData->setName(value);
  return true;
}

QString StringPropertyDataNameEditable::getValue() const
{
  if (mData)
    return mData->getName();
  return "";
}

void StringPropertyDataNameEditable::setData(DataPtr data)
{
  mData = data;
  emit changed();
}

//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------

StringPropertyDataUidEditable::StringPropertyDataUidEditable()
{
}

QString StringPropertyDataUidEditable::getDisplayName() const
{
  return "Uid";
}

bool StringPropertyDataUidEditable::setValue(const QString& value)
{
  return false;
}

QString StringPropertyDataUidEditable::getValue() const
{
  if (mData)
    return mData->getUid();
  return "";
}

void StringPropertyDataUidEditable::setData(DataPtr data)
{
  mData = data;
  emit changed();
}

//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------

StringPropertyDataModality::StringPropertyDataModality(PatientModelServicePtr patientModelService) :
	mPatientModelService(patientModelService)
{
	connect(mPatientModelService.get(), &PatientModelService::dataAddedOrRemoved, this, &Property::changed);
}

StringPropertyDataModality::~StringPropertyDataModality()
{
	disconnect(mPatientModelService.get(), &PatientModelService::dataAddedOrRemoved, this, &Property::changed);
}

void StringPropertyDataModality::setData(ImagePtr data)
{
	if (mData)
		disconnect(mData.get(), &Data::propertiesChanged, this, &Property::changed);
	mData = data;
	if (mData)
		connect(mData.get(), &Data::propertiesChanged, this, &Property::changed);
	emit changed();
}

QString StringPropertyDataModality::getDisplayName() const
{
	return "Modality";
}

bool StringPropertyDataModality::setValue(const QString& value)
{
	if (!mData)
		return false;
	mData->setModality(value);
	return true;
}

QString StringPropertyDataModality::getValue() const
{
	if (!mData)
		return "";
	return mData->getModality();
}

QString StringPropertyDataModality::getHelp() const
{
	if (!mData)
		return "";
	return "Select the modality for " + qstring_cast(mData->getName()) + ".";
}

QStringList StringPropertyDataModality::getValueRange() const
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

StringPropertyImageType::StringPropertyImageType(PatientModelServicePtr patientModelService) :
	mPatientModelService(patientModelService)
{
	connect(mPatientModelService.get(), &PatientModelService::dataAddedOrRemoved, this, &Property::changed);
}

void StringPropertyImageType::setData(ImagePtr data)
{
	if (mData)
		disconnect(mData.get(), &Data::propertiesChanged, this, &Property::changed);
	mData = data;
	if (mData)
		connect(mData.get(), &Data::propertiesChanged, this, &Property::changed);
	emit changed();
}

StringPropertyImageType::~StringPropertyImageType()
{
	disconnect(mPatientModelService.get(), &PatientModelService::dataAddedOrRemoved, this, &Property::changed);
}

QString StringPropertyImageType::getDisplayName() const
{
	return "Image Type";
}

bool StringPropertyImageType::setValue(const QString& value)
{
	if (!mData)
		return false;
	mData->setImageType(value);
	return true;
}

QString StringPropertyImageType::getValue() const
{
	if (!mData)
		return "";
	return mData->getImageType();
}

QString StringPropertyImageType::getHelp() const
{
	if (!mData)
		return "";
	return "Select the image type for " + qstring_cast(mData->getName()) + ".";
}

QStringList StringPropertyImageType::getValueRange() const
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

//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------

StringPropertyGlyphOrientationArray::StringPropertyGlyphOrientationArray(PatientModelServicePtr patientModelService) :
    mPatientModelService(patientModelService)
{
    connect(mPatientModelService.get(), &PatientModelService::dataAddedOrRemoved, this, &Property::changed);
}

StringPropertyGlyphOrientationArray::~StringPropertyGlyphOrientationArray()
{
    disconnect(mPatientModelService.get(), &PatientModelService::dataAddedOrRemoved, this, &Property::changed);
}

void StringPropertyGlyphOrientationArray::setData(MeshPtr data)
{
    if (mData)
        disconnect(mData.get(), &Data::propertiesChanged, this, &Property::changed);
    mData = data;
    if (mData)
        connect(mData.get(), &Data::propertiesChanged, this, &Property::changed);
    emit changed();
}

QString StringPropertyGlyphOrientationArray::getDisplayName() const
{
    return "Set glyph orientation array";
}

bool StringPropertyGlyphOrientationArray::setValue(const QString& value)
{
    if (!mData)
        return false;
    mData->setOrientationArray(value.toStdString().c_str());
    return true;
}

QString StringPropertyGlyphOrientationArray::getValue() const
{
    if (!mData)
        return "";
    return mData->getOrientationArray();
}

QString StringPropertyGlyphOrientationArray::getHelp() const
{
    if (!mData)
        return "";
    return "Select which array to use for orientation of the glyphs.";
}

QStringList StringPropertyGlyphOrientationArray::getValueRange() const
{
    if (!mData)
    {
        QStringList retval;
        retval << "";
        return retval;
    }
    return mData->getOrientationArrayList();
}
//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------

StringPropertyGlyphColorArray::StringPropertyGlyphColorArray(PatientModelServicePtr patientModelService) :
    mPatientModelService(patientModelService)
{
    connect(mPatientModelService.get(), &PatientModelService::dataAddedOrRemoved, this, &Property::changed);
}

StringPropertyGlyphColorArray::~StringPropertyGlyphColorArray()
{
    disconnect(mPatientModelService.get(), &PatientModelService::dataAddedOrRemoved, this, &Property::changed);
}

void StringPropertyGlyphColorArray::setData(MeshPtr data)
{
    if (mData)
        disconnect(mData.get(), &Data::propertiesChanged, this, &Property::changed);
    mData = data;
    if (mData)
        connect(mData.get(), &Data::propertiesChanged, this, &Property::changed);
    emit changed();
}

QString StringPropertyGlyphColorArray::getDisplayName() const
{
    return "Set glyph color array";
}

bool StringPropertyGlyphColorArray::setValue(const QString& value)
{
    if (!mData)
        return false;
    mData->setColorArray(value.toStdString().c_str());
    return true;
}

QString StringPropertyGlyphColorArray::getValue() const
{
    if (!mData)
        return "";
    return mData->getColorArray();
}

QString StringPropertyGlyphColorArray::getHelp() const
{
    if (!mData)
        return "";
    return "Select which array to use for coloring the glyphs.";
}

QStringList StringPropertyGlyphColorArray::getValueRange() const
{
    if (!mData)
    {
        QStringList retval;
        retval << "";
        return retval;
    }
    return mData->getColorArrayList();
}

//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------

StringPropertyGlyphLUT::StringPropertyGlyphLUT(PatientModelServicePtr patientModelService) :
    mPatientModelService(patientModelService)
{
    connect(mPatientModelService.get(), &PatientModelService::dataAddedOrRemoved, this, &Property::changed);
}

StringPropertyGlyphLUT::~StringPropertyGlyphLUT()
{
    disconnect(mPatientModelService.get(), &PatientModelService::dataAddedOrRemoved, this, &Property::changed);
}

void StringPropertyGlyphLUT::setData(MeshPtr data)
{
    if (mData)
        disconnect(mData.get(), &Data::propertiesChanged, this, &Property::changed);
    mData = data;
    if (mData)
        connect(mData.get(), &Data::propertiesChanged, this, &Property::changed);
    emit changed();
}

QString StringPropertyGlyphLUT::getDisplayName() const
{
    return "Set glyph color LUT";
}

bool StringPropertyGlyphLUT::setValue(const QString& value)
{
    if (!mData)
        return false;
    mData->setGlyphLUT(value.toStdString().c_str());
    return true;
}

QString StringPropertyGlyphLUT::getValue() const
{
    if (!mData)
        return "";
    return mData->getGlyphLUT();
}

QString StringPropertyGlyphLUT::getHelp() const
{
    if (!mData)
        return "";
    return "Select which color LUT to use for coloring the glyphs.";
}

QStringList StringPropertyGlyphLUT::getValueRange() const
{
    QStringList retval;

    retval <<
    "Spectrum"<<
    "Warm"<<
    "Cool"<<
    "Blues"<<
    "Wild Flower"<<
    "Citrus"<<

    "Brewer Diverging Purple-Orange"<<
    "Brewer Diverging Spectral"<<
    "Brewer Diverging Brown-Blue-Green"<<

    "Brewer Sequential Blue-Green"<<
    "Brewer Sequential Yellow-Orange-Brown"<<
    "Brewer Sequential Blue-Purple"<<

    "Brewer Qualitative Accent"<<
    "Brewer Qualitative Dark2"<<
    "Brewer Qualitative Set2"<<
    "Brewer Qualitative Pastel2"<<
    "Brewer Qualitative Pastel1"<<
    "Brewer Qualitative Set1"<<
    "Brewer Qualitative Paired"<<
    "Brewer Qualitative Set3";

    return retval;
}

} // namespace cx

