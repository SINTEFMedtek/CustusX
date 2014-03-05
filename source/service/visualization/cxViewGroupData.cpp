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

#include "cxViewGroupData.h"

#include <QMenu>
#include "vtkCamera.h"
#include "sscMessageManager.h"
#include "sscDataManager.h"
#include "cxViewGroup.h" //for class Navigation
#include "sscMesh.h"
#include "sscTypeConversions.h"
#include "cxCameraControl.h"
#include "sscImageAlgorithms.h"
#include "sscDataMetric.h"
#include "sscView.h"
#include "sscImage.h"
#include "cxViewManager.h"
#include "cxInteractiveClipper.h"
#include "cxRepManager.h"
#include "cxVisualizationServiceBackend.h"
#include "cxViewWrapper.h"
#include "boost/bind.hpp"
#include "cxXMLNodeWrapper.h"

namespace cx
{

DataViewProperties DataViewProperties::createDefault()
{
	DataViewProperties retval;
	retval.mVolume3D = true;
	retval.mSlice3D = false;
	retval.mSlice2D = true;
	return retval;
}
DataViewProperties DataViewProperties::createFull()
{
	DataViewProperties retval;
	retval.mVolume3D = true;
	retval.mSlice3D = true;
	retval.mSlice2D = true;
	return retval;
}

DataViewProperties DataViewProperties::createVolume3D()
{
	DataViewProperties retval;
	retval.mVolume3D = true;
	retval.mSlice3D = false;
	retval.mSlice2D = false;
	return retval;
}

DataViewProperties DataViewProperties::createSlice3D()
{
	DataViewProperties retval;
	retval.mVolume3D = false;
	retval.mSlice3D = true;
	retval.mSlice2D = false;
	return retval;
}

DataViewProperties DataViewProperties::createSlice2D()
{
	DataViewProperties retval;
	retval.mVolume3D = false;
	retval.mSlice3D = false;
	retval.mSlice2D = true;
	return retval;
}

DataViewProperties DataViewProperties::create3D()
{
	DataViewProperties retval;
	retval.mVolume3D = true;
	retval.mSlice3D = true;
	retval.mSlice2D = false;
	return retval;
}


void DataViewProperties::addXml(QDomNode& dataNode)
{
	QDomElement elem = dataNode.toElement();
	elem.setAttribute("volume3D", mVolume3D);
	elem.setAttribute("slice3D", mSlice3D);
	elem.setAttribute("slice2D", mSlice2D);
}

void DataViewProperties::parseXml(QDomNode dataNode)
{
	QDomElement elem = dataNode.toElement();
	mVolume3D = elem.attribute("volume3D", QString::number(mVolume3D)).toInt();
	mSlice3D = elem.attribute("slice3D", QString::number(mSlice3D)).toInt();
	mSlice2D = elem.attribute("slice2D", QString::number(mSlice2D)).toInt();
}

bool DataViewProperties::empty() const
{
	return !(mVolume3D || mSlice3D || mSlice2D);
}

DataViewProperties DataViewProperties::addFlagsIn(DataViewProperties rhs) const
{
	DataViewProperties retval;
	retval.mSlice2D = mSlice2D || rhs.mSlice2D;
	retval.mSlice3D = mSlice3D || rhs.mSlice3D;
	retval.mVolume3D = mVolume3D || rhs.mVolume3D;
	return retval;
}

DataViewProperties DataViewProperties::removeFlagsIn(DataViewProperties rhs) const
{
	DataViewProperties retval;
	retval.mSlice2D = mSlice2D && !rhs.mSlice2D;
	retval.mSlice3D = mSlice3D && !rhs.mSlice3D;
	retval.mVolume3D = mVolume3D && !rhs.mVolume3D;
	return retval;
}

bool DataViewProperties::containsAnyFlagsIn(DataViewProperties required) const
{
	if (required.mSlice2D && mSlice2D) return true;
	if (required.mSlice3D && mSlice3D) return true;
	if (required.mVolume3D && mVolume3D) return true;
	return false;
}



///--------------------------------------------------------
///--------------------------------------------------------
///--------------------------------------------------------

bool dataTypeSort(const DataPtr data1, const DataPtr data2)
{
	return getPriority(data1) < getPriority(data2);
}

int getPriority(DataPtr data)
{
	if (data->getType()=="mesh")
		return 6;
	DataMetricPtr metric = boost::dynamic_pointer_cast<DataMetric>(data);
	if (metric)
		return 7;

	ImagePtr image = boost::dynamic_pointer_cast<Image>(data);
	if (image)
	{
		if (image->getModality().toUpper().contains("US"))
		{
			if (image->getImageType().toUpper().contains("B-MODE"))
				return 4;
			else // angio types
				return 5;
		}
		else if (image->getModality().toUpper().contains("MR"))
		{
			// MR, CT, SC, others
			return 2;
		}
		else if (image->getModality().toUpper().contains("CT"))
		{
			// MR, CT, SC, others
			return 1;
		}
		else
		{
			return 0;
		}
	}

	return 3;
}

///----------------------------------------------------------------------------
///----------------------------------------------------------------------------
///----------------------------------------------------------------------------

ViewGroupData::Options::Options() :
	mShowLandmarks(false), mShowPointPickerProbe(false),
	mPickerGlyph(new Mesh("PickerGlyph"))
{
}

ViewGroupData::ViewGroupData(VisualizationServiceBackendPtr backend) :
	mBackend(backend),
	mCamera3D(CameraData::create())
{
	if(mBackend)
		connect(mBackend->getDataManager(), SIGNAL(dataAddedOrRemoved()), this, SLOT(dataAddedOrRemovedInManager()));
	mVideoSource = "active";
	mGroup2DZoom = SyncedValue::create(1);
	mGlobal2DZoom = mGroup2DZoom;

	mSliceDefinitions.add(ptAXIAL);
	mSliceDefinitions.add(ptCORONAL);
	mSliceDefinitions.add(ptSAGITTAL);
}

void ViewGroupData::dataAddedOrRemovedInManager()
{
	for (unsigned i = 0; i < mData.size(); )
	{
		if (!mBackend->getDataManager()->getData(mData[i].first->getUid()))
			this->removeData(mData[i].first);
		else
			++i;
	}
}

void ViewGroupData::requestInitialize()
{
	emit initialized();
}

void ViewGroupData::addData(DataPtr data)
{
	DataViewProperties properties = this->getProperties(data);
	properties.addFlagsIn(DataViewProperties::createDefault());
	this->setProperties(data, properties);
}

void ViewGroupData::addDataSorted(DataPtr data)
{
	if (!data)
		return;
	if (this->contains(data))
		return;

	DataViewProperties properties = DataViewProperties::createDefault();
	DataAndViewProperties item(data, properties);

	for (int i=mData.size()-1; i>=0; --i)
	{
		if (!dataTypeSort(data, mData[i].first))
		{
			mData.insert(mData.begin()+i+1, item);
			break;
		}
	}
	if (!this->contains(data))
		mData.insert(mData.begin(), item);
	emit dataAdded(qstring_cast(data->getUid()));
}

DataViewProperties ViewGroupData::getProperties(DataPtr data)
{
	if (this->contains(data))
		return std::find_if(mData.begin(), mData.end(), data_equals(data))->second;
	return DataViewProperties();
}

void ViewGroupData::setProperties(DataPtr data, DataViewProperties properties)
{
	if (!data)
		return;

	if (properties.empty())
	{
		this->removeData(data);
		return;
	}

	if (!this->contains(data))
	{
		DataAndViewProperties item(data, properties);
		mData.push_back(item);
	}
	else
	{
		std::find_if(mData.begin(), mData.end(), data_equals(data))->second = properties;
	}

	emit dataAdded(data->getUid());
	emit dataRemoved(data->getUid());
}

bool ViewGroupData::contains(DataPtr data) const
{
	return std::count_if(mData.begin(), mData.end(), data_equals(data));
}

bool ViewGroupData::removeData(DataPtr data)
{
	if (!data)
		return false;
	if (!this->contains(data))
		return false;
	mData.erase(std::find_if(mData.begin(), mData.end(), data_equals(data)));
	emit dataRemoved(qstring_cast(data->getUid()));
	return true;
}

void ViewGroupData::clearData()
{
	while (!mData.empty())
		this->removeData(mData.front().first);
	this->setVideoSource("active");

	mGroup2DZoom->set(1.0);
	mGlobal2DZoom->set(1.0);
}

void ViewGroupData::setVideoSource(QString uid)
{
	if (mVideoSource==uid)
		return;
	mVideoSource = uid;
	emit videoSourceChanged(mVideoSource);
}

QString ViewGroupData::getVideoSource() const
{
	return mVideoSource;
}

std::vector<DataPtr> ViewGroupData::getData(DataViewProperties properties) const
{
	return this->getDataOfType<Data>(properties);
}

template<class DATA_TYPE>
std::vector<boost::shared_ptr<DATA_TYPE> > ViewGroupData::getDataOfType(DataViewProperties requiredProperties) const
{
	typedef boost::shared_ptr<DATA_TYPE> DATA_PTR;
	std::vector<DATA_PTR> retval;
	for (unsigned i = 0; i < mData.size(); ++i)
	{
		DATA_PTR data = boost::dynamic_pointer_cast<DATA_TYPE>(mData[i].first);
		if (!data)
			continue;
		DataViewProperties properties = mData[i].second;
		if (!properties.containsAnyFlagsIn(requiredProperties))
			continue;
		retval.push_back(data);
	}
	return retval;
}

std::vector<ImagePtr> ViewGroupData::getImages(DataViewProperties properties) const
{
	return this->getDataOfType<Image>(properties);
}

std::vector<MeshPtr> ViewGroupData::getMeshes(DataViewProperties properties) const
{
	return this->getDataOfType<Mesh>(properties);
}

ViewGroupData::Options ViewGroupData::getOptions() const
{
	return mOptions;
}

void ViewGroupData::setOptions(ViewGroupData::Options options)
{
	mOptions = options;
	emit optionsChanged();
}

void ViewGroupData::initializeGlobal2DZoom(SyncedValuePtr val)
{
	mGlobal2DZoom = val;
}

SyncedValuePtr ViewGroupData::getGroup2DZoom()
{
	return mGroup2DZoom;
}
SyncedValuePtr ViewGroupData::getGlobal2DZoom()
{
	return mGlobal2DZoom;
}

void ViewGroupData::addXml(QDomNode& dataNode)
{
	XMLNodeAdder base(dataNode);

	for (unsigned i = 0; i < mData.size(); ++i)
	{
		QDomElement elem;
		elem = base.addTextToElement("data", mData[i].first->getUid());
		mData[i].second.addXml(elem);
	}

	base.addObjectToElement("camera3D", this->getCamera3D());
	base.addTextToElement("slicesPlanes3D", mSliceDefinitions.toString());
}

void ViewGroupData::parseXml(QDomNode dataNode)
{
	XMLNodeParser base(dataNode);

	QString sliceText = base.parseTextFromElement("slicesPlanes3D");
	mSliceDefinitions = PlaneTypeCollection::fromString(sliceText, mSliceDefinitions);

	std::vector<QDomElement> dataElems = base.getDuplicateElements("data");
	for (unsigned i=0; i<dataElems.size(); ++i)
	{
		QDomElement elem = dataElems[i];
		QString uid = elem.text();
		DataPtr data = mBackend->getDataManager()->getData(uid);
		if (!data)
		{
			messageManager()->sendError("Couldn't find the data: [" + uid + "] in the datamanager.");
			continue;
		}
		DataViewProperties properties = DataViewProperties::createDefault();
		properties.parseXml(elem);

		this->addData(data);
		this->setProperties(data, properties);
	}

	base.parseObjectFromElement("camera3D", this->getCamera3D());
}

} // namespace cx
