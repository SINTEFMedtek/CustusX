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

namespace cx
{

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
}

void ViewGroupData::dataAddedOrRemovedInManager()
{
	for (unsigned i = 0; i < mData.size(); )
	{
		if (!mBackend->getDataManager()->getData(mData[i]->getUid()))
			this->removeData(mData[i]);
		else
			++i;
	}
}

void ViewGroupData::requestInitialize()
{
	emit initialized();
}

std::vector<DataPtr> ViewGroupData::getData() const
{
	return mData;
}

void ViewGroupData::addData(DataPtr data)
{
	if (!data)
		return;
	if (std::count(mData.begin(), mData.end(), data))
		return;
	mData.push_back(data);
	emit dataAdded(qstring_cast(data->getUid()));
}

void ViewGroupData::addDataSorted(DataPtr data)
{
	if (!data)
		return;
	if (std::count(mData.begin(), mData.end(), data))
		return;
	for (int i=mData.size()-1; i>=0; --i)
	{
		if (!dataTypeSort(data, mData[i]))
		{
			mData.insert(mData.begin()+i+1, data);
			break;
		}
	}
	if (!std::count(mData.begin(), mData.end(), data))
		mData.insert(mData.begin(), data);
	emit dataAdded(qstring_cast(data->getUid()));
}

bool ViewGroupData::removeData(DataPtr data)
{
	if (!data)
		return false;
	if (!std::count(mData.begin(), mData.end(), data))
		return false;
	mData.erase(std::find(mData.begin(), mData.end(), data));
	emit dataRemoved(qstring_cast(data->getUid()));
	return true;
}

void ViewGroupData::clearData()
{
	while (!mData.empty())
		this->removeData(mData.front());
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

std::vector<ImagePtr> ViewGroupData::getImages() const
{
	std::vector<ImagePtr> retval;
	for (unsigned i = 0; i < mData.size(); ++i)
	{
		ImagePtr data = boost::dynamic_pointer_cast<Image>(mData[i]);
		if (data)
			retval.push_back(data);
	}
	return retval;
}

std::vector<MeshPtr> ViewGroupData::getMeshes() const
{
	std::vector<MeshPtr> retval;
	for (unsigned i = 0; i < mData.size(); ++i)
	{
		MeshPtr data = boost::dynamic_pointer_cast<Mesh>(mData[i]);
		if (data)
			retval.push_back(data);
	}
	return retval;
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


} // namespace cx
