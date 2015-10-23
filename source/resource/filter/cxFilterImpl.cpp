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

