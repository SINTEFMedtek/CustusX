/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxSelectDataStringProperty.h"

#include "cxImage.h"
#include "cxMesh.h"
#include "cxTrackedStream.h"
#include "cxTypeConversions.h"
#include "cxPatientModelServiceProxy.h"
#include "cxActiveData.h"

namespace cx
{

StringPropertyActiveData::StringPropertyActiveData(PatientModelServicePtr patientModelService, QString typeRegexp) :
	SelectDataStringPropertyBase(patientModelService, typeRegexp)
{
	mValueName = "Active Data";
	mHelp = "Select the active data obejct";

	mActiveData = mPatientModelService->getActiveData();
	connect(mActiveData.get(), &ActiveData::activeDataChanged, this, &Property::changed);
	connect(mActiveData.get(), &ActiveData::activeDataChanged, this, &SelectDataStringPropertyBase::dataChanged);
}

bool StringPropertyActiveData::setValue(const QString& value)
{
	DataPtr newData = mPatientModelService->getData(value);
	if (newData == mActiveData->getActive())
		return false;
	mActiveData->setActive(newData);
	return true;
}

QString StringPropertyActiveData::getValue() const
{
	QString retval = "";
	DataPtr activeData = mActiveData->getActiveUsingRegexp(mTypeRegexp);

	if(activeData)
		retval = activeData->getUid();
	return retval;
}

StringPropertyActiveImage::StringPropertyActiveImage(PatientModelServicePtr patientModelService) :
	SelectDataStringPropertyBase(patientModelService, Image::getTypeName())
{
	mValueName = "Active Volume";
	mHelp = "Select the active volume";

	mActiveData = mPatientModelService->getActiveData();
	connect(mActiveData.get(), &ActiveData::activeImageChanged, this, &Property::changed);
	connect(mActiveData.get(), &ActiveData::activeImageChanged, this, &SelectDataStringPropertyBase::dataChanged);
}

bool StringPropertyActiveImage::setValue(const QString& value)
{
  ImagePtr newImage = mPatientModelService->getData<Image>(value);
  if (newImage==mActiveData->getActive<Image>())
	return false;
  mActiveData->setActive(newImage);
  return true;
}

QString StringPropertyActiveImage::getValue() const
{
	return mActiveData->getActiveImageUid();
}

//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------

StringPropertySelectImage::StringPropertySelectImage(PatientModelServicePtr patientModelService) :
	SelectDataStringPropertyBase(patientModelService, Image::getTypeName())
{
	mValueName = "Select volume";
	mHelp = "Select a volume";
}

bool StringPropertySelectImage::setValue(const QString& value)
{
  if (value==mImageUid)
	return false;
  mImageUid = value;
  emit changed();
  emit dataChanged(mImageUid);
  return true;
}

QString StringPropertySelectImage::getValue() const
{
  return mImageUid;
}

ImagePtr StringPropertySelectImage::getImage()
{
  return mPatientModelService->getData<Image>(mImageUid);
}

//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------


StringPropertySelectData::StringPropertySelectData(PatientModelServicePtr patientModelService, QString typeRegexp) :
	SelectDataStringPropertyBase(patientModelService, typeRegexp)
{
}

bool StringPropertySelectData::setValue(const QString& value)
{
  if (value==mUid)
	return false;

  mUid = "";
  if (mPatientModelService->getData(value))
	  mUid = value;

  emit changed();
  emit dataChanged(this->getValue());
  return true;
}

QString StringPropertySelectData::getValue() const
{
	return mUid;
//  if(!mData)
//    return "<no data>";
//  return mData->getUid();
}

DataPtr StringPropertySelectData::getData() const
{
	return mPatientModelService->getData(mUid);
//  return mData;
}

//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------

StringPropertySelectMesh::StringPropertySelectMesh(PatientModelServicePtr patientModelService) :
	SelectDataStringPropertyBase(patientModelService, Mesh::getTypeName())
{
	mValueName = "Select mesh";
	mHelp = "Select a mesh";
}

bool StringPropertySelectMesh::setValue(const QString& value)
{
  if (value==mMeshUid)
	return false;
  mMeshUid = value;
  emit changed();
  emit dataChanged(mMeshUid);
  return true;
}

QString StringPropertySelectMesh::getValue() const
{
  return mMeshUid;
}

MeshPtr StringPropertySelectMesh::getMesh()
{
  return mPatientModelService->getData<Mesh>(mMeshUid);
}

//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------

StringPropertySelectTrackedStream::StringPropertySelectTrackedStream(PatientModelServicePtr patientModelService) :
	SelectDataStringPropertyBase(patientModelService, TrackedStream::getTypeName())
{
	mValueName = "Select stream";
	mHelp = "Select a tracked stream";
}

bool StringPropertySelectTrackedStream::setValue(const QString& value)
{
  if (value==mStreamUid)
	return false;
  mStreamUid = value;
  emit changed();
  emit dataChanged(mStreamUid);
  return true;
}

QString StringPropertySelectTrackedStream::getValue() const
{
  return mStreamUid;
}

TrackedStreamPtr StringPropertySelectTrackedStream::getTrackedStream()
{
  return mPatientModelService->getData<TrackedStream>(mStreamUid);
}

//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------

} // namespace cx

