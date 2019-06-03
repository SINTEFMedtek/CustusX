/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#include "cxDataInterface.h"
#include "cxImage.h"
#include <QSet>
#include "cxMesh.h"
#include "cxImageLUT2D.h"
#include "cxTrackingService.h"
#include "cxTypeConversions.h"
#include "cxTool.h"
#include "cxImageAlgorithms.h"
#include "cxRegistrationTransform.h"
#include "cxLogger.h"
#include "cxActiveImageProxy.h"
#include "cxVideoSource.h"
#include "cxVideoService.h"
#include "cxPatientModelService.h"
#include "cxActiveToolProxy.h"
#include "cxActiveData.h"
#include "cxEnumConversion.h"

namespace cx
{
DoublePropertyActiveToolOffset::DoublePropertyActiveToolOffset(ActiveToolProxyPtr activeTool) :
	mActiveTool(activeTool)
{
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

DoublePropertyToolOffset::DoublePropertyToolOffset(ToolPtr tool) :
	mTool(tool)
{
	this->setTool(tool);
}

void DoublePropertyToolOffset::setTool(ToolPtr tool)
{
	if (mTool)
		disconnect(mTool.get(), &Tool::tooltipOffset, this, &Property::changed);
	mTool = tool;
	if (mTool)
		connect(mTool.get(), &Tool::tooltipOffset, this, &Property::changed);
	emit changed();
}

double DoublePropertyToolOffset::getValue() const
{
	return mTool->getTooltipOffset();
}

bool DoublePropertyToolOffset::setValue(double val)
{
	mTool->setTooltipOffset(val);
	return true;
}

DoubleRange DoublePropertyToolOffset::getValueRange() const
{
	double range = 200;
	return DoubleRange(0,range,1);
}


//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------

DoublePropertyActiveImageBase::DoublePropertyActiveImageBase(ActiveDataPtr activeData) :
	mActiveData(activeData)
{
	mActiveImageProxy = ActiveImageProxy::New(mActiveData);
	connect(mActiveImageProxy.get(), &ActiveImageProxy::activeImageChanged, this, &DoublePropertyActiveImageBase::activeImageChanged);
	connect(mActiveImageProxy.get(), &ActiveImageProxy::transferFunctionsChanged, this, &Property::changed);
}
void DoublePropertyActiveImageBase::activeImageChanged()
{
	mImage = mActiveData->getActive<Image>();
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

	std::map<QString, DataPtr> allData = mPatientModelService->getDatas();
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
	emit parentChanged();
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
	mData->setModality(string2enum<IMAGE_MODALITY>(value));
	return true;
}

QString StringPropertyDataModality::getValue() const
{
	if (!mData)
		return "";
	return enum2string(mData->getModality());
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
		retval << enum2string(mData->getModality());
	retval << enum2string(imCT) << enum2string(imMR) << enum2string(imUS);
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
	mData->setImageType(string2enum<IMAGE_SUBTYPE>(value));
	return true;
}

QString StringPropertyImageType::getValue() const
{
	if (!mData)
		return "";
	return enum2string(mData->getImageType());
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
		retval << enum2string(mData->getImageType());
		if (mData->getModality() == imCT)
			retval << enum2string(istSEGMENTATION);
		if (mData->getModality() == imMR)
			retval << enum2string(istMRT1) << enum2string(istMRT2) << enum2string(istMRFLAIR) << enum2string(istANGIO) << enum2string(istSEGMENTATION);
		if (mData->getModality() == imUS)
			retval << enum2string(istUSBMODE) << enum2string(istANGIO) << enum2string(istSEGMENTATION);
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

