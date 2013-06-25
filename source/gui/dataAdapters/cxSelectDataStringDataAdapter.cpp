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

#include "cxSelectDataStringDataAdapter.h"

#include "sscImage.h"
#include "sscImageAlgorithms.h"
#include "sscMesh.h"
#include "sscDataManager.h"
#include "sscTypeConversions.h"

namespace cx
{

SelectDataStringDataAdapterBase::SelectDataStringDataAdapterBase(QString typeRegexp) :
	mTypeRegexp(typeRegexp)
{
	mValueName = "Select data";
	mHelp = mValueName;
	connect(ssc::dataManager(), SIGNAL(dataLoaded()),   this, SIGNAL(changed()));
}

/**
  * Erase all data with type not conforming to input regexp.
  */
std::map<QString, ssc::DataPtr> SelectDataStringDataAdapterBase::filterOnType(std::map<QString, ssc::DataPtr> input, QString regexp) const
{
	QRegExp reg(regexp);

	std::map<QString, ssc::DataPtr>::iterator iter, current;

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
  std::map<QString, ssc::DataPtr> data = ssc::dataManager()->getData();
  data = this->filterOnType(data, mTypeRegexp);
  std::vector<ssc::DataPtr> sorted = sortOnGroupsAndAcquisitionTime(data);
  QStringList retval;
  retval << "";
  for (unsigned i=0; i<sorted.size(); ++i)
	retval << sorted[i]->getUid();
  return retval;
}

QString SelectDataStringDataAdapterBase::convertInternal2Display(QString internal)
{
  ssc::DataPtr data = ssc::dataManager()->getData(internal);
  if (!data)
	return "<no data>";
  return qstring_cast(data->getName());
}

QString SelectDataStringDataAdapterBase::getHelp() const
{
  return mHelp;
}

QString SelectDataStringDataAdapterBase::getValueName() const
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

ssc::DataPtr SelectDataStringDataAdapterBase::getData() const
{
	return ssc::dataManager()->getData(this->getValue());
}

//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------

ActiveImageStringDataAdapter::ActiveImageStringDataAdapter() : SelectDataStringDataAdapterBase("image")
{
  mValueName = "Active Volume";
  mHelp = "Select the active volume";
  connect(ssc::dataManager(), SIGNAL(activeImageChanged(QString)),      this, SIGNAL(changed()));
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

ssc::ImagePtr SelectImageStringDataAdapter::getImage()
{
  return ssc::dataManager()->getImage(mImageUid);
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
  if (ssc::dataManager()->getData(value))
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

ssc::DataPtr SelectDataStringDataAdapter::getData() const
{
	return ssc::dataManager()->getData(mUid);
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

ssc::MeshPtr SelectMeshStringDataAdapter::getMesh()
{
  return ssc::dataManager()->getMesh(mMeshUid);
}

//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------


} // namespace cx

