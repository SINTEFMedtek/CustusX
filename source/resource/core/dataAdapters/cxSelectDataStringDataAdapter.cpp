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

#include "cxSelectDataStringDataAdapter.h"

#include "cxImage.h"
#include "cxMesh.h"
#include "cxTypeConversions.h"
#include "cxPatientModelServiceProxy.h"

namespace cx
{

ActiveImageStringDataAdapter::ActiveImageStringDataAdapter(PatientModelServicePtr patientModelService) :
	SelectDataStringDataAdapterBase(patientModelService, "image")
{
	mValueName = "Active Volume";
	mHelp = "Select the active volume";
	connect(mPatientModelService.get(), SIGNAL(activeImageChanged(QString)), this, SIGNAL(changed()));
}

bool ActiveImageStringDataAdapter::setValue(const QString& value)
{
  ImagePtr newImage = mPatientModelService->getImage(value);
  if (newImage==mPatientModelService->getActiveImage())
	return false;
  mPatientModelService->setActiveImage(newImage);
  return true;
}

QString ActiveImageStringDataAdapter::getValue() const
{
  if (!mPatientModelService->getActiveImage())
	return "";
  return qstring_cast(mPatientModelService->getActiveImage()->getUid());
}

//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------

SelectImageStringDataAdapter::SelectImageStringDataAdapter(PatientModelServicePtr patientModelService) :
	SelectDataStringDataAdapterBase(patientModelService, "image")
{
	mValueName = "Select volume";
	mHelp = "Select a volume";
}

bool SelectImageStringDataAdapter::setValue(const QString& value)
{
  if (value==mImageUid)
	return false;
  mImageUid = value;
  emit changed();
  emit dataChanged(mImageUid);
  return true;
}

QString SelectImageStringDataAdapter::getValue() const
{
  return mImageUid;
}

ImagePtr SelectImageStringDataAdapter::getImage()
{
  return mPatientModelService->getImage(mImageUid);
}

//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------


SelectDataStringDataAdapter::SelectDataStringDataAdapter(PatientModelServicePtr patientModelService) :
	SelectDataStringDataAdapterBase(patientModelService, ".*")
{
}

bool SelectDataStringDataAdapter::setValue(const QString& value)
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

QString SelectDataStringDataAdapter::getValue() const
{
	return mUid;
//  if(!mData)
//    return "<no data>";
//  return mData->getUid();
}

DataPtr SelectDataStringDataAdapter::getData() const
{
	return mPatientModelService->getData(mUid);
//  return mData;
}

//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------

SelectMeshStringDataAdapter::SelectMeshStringDataAdapter(PatientModelServicePtr patientModelService) :
	SelectDataStringDataAdapterBase(patientModelService, "mesh")
{
	mValueName = "Select mesh";
	mHelp = "Select a mesh";
}

bool SelectMeshStringDataAdapter::setValue(const QString& value)
{
  if (value==mMeshUid)
	return false;
  mMeshUid = value;
  emit changed();
  emit dataChanged(mMeshUid);
  return true;
}

QString SelectMeshStringDataAdapter::getValue() const
{
  return mMeshUid;
}

MeshPtr SelectMeshStringDataAdapter::getMesh()
{
  return mPatientModelService->getMesh(mMeshUid);
}

//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------


} // namespace cx

