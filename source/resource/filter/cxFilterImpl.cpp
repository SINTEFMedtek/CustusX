/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxFilterImpl.h"

#include "cxImage.h"
#include "cxSelectDataStringProperty.h"
#include "cxDoubleProperty.h"
#include "cxDoublePairProperty.h"
#include "cxStringProperty.h"
#include "cxPatientModelService.h"
#include "cxVisServices.h"

namespace cx
{

FilterImpl::FilterImpl(VisServicesPtr services) :
	mActive(false), mServices(services)
{
}

PatientModelServicePtr FilterImpl::patientService()
{
	return mServices->patient();
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

std::vector<PropertyPtr> FilterImpl::getOptions()
{
	if (mOptionsAdapters.empty())
	{
		this->createOptions();
	}
	return mOptionsAdapters;
}

std::vector<SelectDataStringPropertyBasePtr> FilterImpl::getInputTypes()
{
	if (mInputTypes.empty())
	{
		this->createInputTypes();
	}

	return mInputTypes;
}

std::vector<SelectDataStringPropertyBasePtr> FilterImpl::getOutputTypes()
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

void FilterImpl::updateThresholdFromImageChange(QString uid, DoublePropertyPtr threshold)
{
	ImagePtr image = mServices->patient()->getData<Image>(uid);
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

void FilterImpl::updateThresholdPairFromImageChange(QString uid, DoublePairPropertyPtr threshold)
{
	ImagePtr image = mServices->patient()->getData<Image>(uid);
	if(!image)
		return;
	threshold->setValueRange(DoubleRange(image->getMin(), image->getMax(), 1));

	int initLower = ::ceil(double(image->getMin()) + double(image->getRange())/10); // round up
	int initUpper = image->getMax();
	threshold->setValue(Eigen::Vector2d(initLower, initUpper));
}


} // namespace cx

