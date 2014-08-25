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
#include "cxImageAlgorithms.h"
#include "cxMesh.h"
#include "cxDataManager.h"
#include "cxTypeConversions.h"

namespace cx
{

SelectDataStringDataAdapterBase::SelectDataStringDataAdapterBase(QString typeRegexp) :
	mTypeRegexp(typeRegexp)
{
	mValueName = "Select data";
	mHelp = mValueName;
	connect(dataManager(), SIGNAL(dataAddedOrRemoved()),   this, SIGNAL(changed()));
}

/**
  * Erase all data with type not conforming to input regexp.
  */
std::map<QString, DataPtr> SelectDataStringDataAdapterBase::filterOnType(std::map<QString, DataPtr> input, QString regexp) const
{
	QRegExp reg(regexp);

	std::map<QString, DataPtr>::iterator iter, current;

	for (iter=input.begin(); iter!=input.end(); )
	{
		current = iter++; // increment iterator before erasing!
		if (!current->second->getType().contains(reg))
			input.erase(current);
	}

	return input;
}

QStringList SelectDataStringDataAdapterBase::getValueRange() const
{
  std::map<QString, DataPtr> data = dataManager()->getData();
  data = this->filterOnType(data, mTypeRegexp);
  std::vector<DataPtr> sorted = sortOnGroupsAndAcquisitionTime(data);
  QStringList retval;
  retval << "";
  for (unsigned i=0; i<sorted.size(); ++i)
	retval << sorted[i]->getUid();
  return retval;
}

QString SelectDataStringDataAdapterBase::convertInternal2Display(QString internal)
{
  DataPtr data = dataManager()->getData(internal);
  if (!data)
	return "<no data>";
  return qstring_cast(data->getName());
}

QString SelectDataStringDataAdapterBase::getHelp() const
{
  return mHelp;
}

QString SelectDataStringDataAdapterBase::getDisplayName() const
{
  return mValueName;
}

void SelectDataStringDataAdapterBase::setHelp(QString text)
{
	mHelp = text;
	emit changed();
}
void SelectDataStringDataAdapterBase::setValueName(QString val)
{
  mValueName = val;
  emit changed();
}

DataPtr SelectDataStringDataAdapterBase::getData() const
{
	return dataManager()->getData(this->getValue());
}

//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------

ActiveImageStringDataAdapter::ActiveImageStringDataAdapter() : SelectDataStringDataAdapterBase("image")
{
  mValueName = "Active Volume";
  mHelp = "Select the active volume";
  connect(dataManager(), SIGNAL(activeImageChanged(QString)),      this, SIGNAL(changed()));
}

bool ActiveImageStringDataAdapter::setValue(const QString& value)
{
  ImagePtr newImage = dataManager()->getImage(value);
  if (newImage==dataManager()->getActiveImage())
	return false;
  dataManager()->setActiveImage(newImage);
  return true;
}

QString ActiveImageStringDataAdapter::getValue() const
{
  if (!dataManager()->getActiveImage())
	return "";
  return qstring_cast(dataManager()->getActiveImage()->getUid());
}

//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------

SelectImageStringDataAdapter::SelectImageStringDataAdapter() : SelectDataStringDataAdapterBase("image")
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
  return dataManager()->getImage(mImageUid);
}

//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------


SelectDataStringDataAdapter::SelectDataStringDataAdapter() : SelectDataStringDataAdapterBase(".*")
{
}

bool SelectDataStringDataAdapter::setValue(const QString& value)
{
  if (value==mUid)
	return false;

  mUid = "";
  if (dataManager()->getData(value))
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
	return dataManager()->getData(mUid);
//  return mData;
}

//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------

SelectMeshStringDataAdapter::SelectMeshStringDataAdapter() :
	SelectDataStringDataAdapterBase("mesh")
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
  return dataManager()->getMesh(mMeshUid);
}

//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------


} // namespace cx

