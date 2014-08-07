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

#include "cxFilterImpl.h"

#include "cxImage.h"
#include "cxDataManager.h"
#include "cxSelectDataStringDataAdapter.h"
#include "cxDoubleDataAdapterXml.h"
#include "cxDoublePairDataAdapterXml.h"
#include "cxStringDataAdapterXml.h"

namespace cx
{

FilterImpl::FilterImpl() : mActive(false)
{
}

QString FilterImpl::getUid() const
{
	if (mUid.isEmpty())
		return this->getType();
	return mUid;
}

void FilterImpl::initialize(QDomElement root, QString uid)
{
	if (!uid.isEmpty())
		mUid = uid;

	mOptions = root;
}

std::vector<DataAdapterPtr> FilterImpl::getOptions()
{
	if (mOptionsAdapters.empty())
	{
		this->createOptions();
	}
	return mOptionsAdapters;
}

std::vector<SelectDataStringDataAdapterBasePtr> FilterImpl::getInputTypes()
{
	if (mInputTypes.empty())
	{
		this->createInputTypes();
	}

	return mInputTypes;
}

std::vector<SelectDataStringDataAdapterBasePtr> FilterImpl::getOutputTypes()
{
	if (mOutputTypes.empty())
	{
		this->createOutputTypes();
	}

	return mOutputTypes;
}

void FilterImpl::setActive(bool on)
{
	mActive = on;
}


bool FilterImpl::preProcess()
{
//	std::cout << "FilterImpl::preProcess " << mInputTypes.size() << std::endl;

	mCopiedInput.clear();
	for (unsigned i=0; i<mInputTypes.size(); ++i)
	{
		mCopiedInput.push_back(mInputTypes[i]->getData());
	}

	mCopiedOptions = mOptions.cloneNode(true).toElement();

	// clear output
	for (unsigned i=0; i<mOutputTypes.size(); ++i)
		mOutputTypes[i]->setValue("");

	return true;
}

ImagePtr FilterImpl::getCopiedInputImage(int index)
{
	if (mCopiedInput.size() < index+1)
		return ImagePtr();
	return boost::dynamic_pointer_cast<Image>(mCopiedInput[index]);
}

void FilterImpl::updateThresholdFromImageChange(QString uid, DoubleDataAdapterXmlPtr threshold)
{
	ImagePtr image = dataManager()->getImage(uid);
	if(!image)
		return;
	threshold->setValueRange(DoubleRange(image->getMin(), image->getMax(), 1));
	int oldLower = threshold->getValue();
	// avoid reset if old value is still within range,
	// but reset anyway if old val is 0..1, this can indicate old image was binary.
	if ((image->getMin() > oldLower )||( oldLower > image->getMax() )||( oldLower<=1 ))
	{
		int initLower = ::ceil(double(image->getMin()) + double(image->getRange())/10); // round up
		threshold->setValue(initLower);
	}
//	std::cout << "FilterImpl::imageChangedSlot " << image->getMin() << " "  << image->getMax() << std::endl;
//	std::cout << "            imageChangedSlot() " << threshold->getValue() << std::endl;
}

void FilterImpl::updateThresholdPairFromImageChange(QString uid, DoublePairDataAdapterXmlPtr threshold)
{
	ImagePtr image = dataManager()->getImage(uid);
	if(!image)
		return;
	threshold->setValueRange(DoubleRange(image->getMin(), image->getMax(), 1));

	int initLower = ::ceil(double(image->getMin()) + double(image->getRange())/10); // round up
	int initUpper = image->getMax();
	threshold->setValue(Eigen::Vector2d(initLower, initUpper));
}


} // namespace cx

