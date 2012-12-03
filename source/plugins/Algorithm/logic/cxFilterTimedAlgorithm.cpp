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

#include "cxFilterTimedAlgorithm.h"

namespace cx
{

FilterTimedAlgorithm::FilterTimedAlgorithm(FilterPtr filter) :
    ThreadedTimedAlgorithm<bool>(filter->getName(), 20)
{
	mFilter = filter;
	mUseDefaultMessages = false;
}

FilterTimedAlgorithm::~FilterTimedAlgorithm()
{}

FilterPtr FilterTimedAlgorithm::getFilter()
{
	return mFilter;
}

void FilterTimedAlgorithm::preProcessingSlot()
{
	mFilter->preProcess();
}

void FilterTimedAlgorithm::postProcessingSlot()
{
	bool success = this->getResult();

	mFilter->postProcess();

	if (success)
	{
		ssc::messageManager()->sendSuccess(QString("Done \"%1\": [%2s]")
		                                   .arg(mFilter->getName())
		                                   .arg(this->getSecondsPassedAsString()));
	}
	else
	{
		ssc::messageManager()->sendWarning(QString("Failed \"%1\": [%2s]")
		                                   .arg(mFilter->getName())
		                                   .arg(this->getSecondsPassedAsString()));
	}
}

bool FilterTimedAlgorithm::calculate()
{
	return mFilter->execute();
}


} // namespace cx
