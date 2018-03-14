/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxFilterTimedAlgorithm.h"
#include "cxLogger.h"
#include "cxFilter.h"

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
		reportSuccess(QString("Done \"%1\": [%2s]")
		                                   .arg(mFilter->getName())
		                                   .arg(this->getSecondsPassedAsString()));
	}
	else
	{
		reportWarning(QString("Failed \"%1\": [%2s]")
		                                   .arg(mFilter->getName())
		                                   .arg(this->getSecondsPassedAsString()));
	}
}

bool FilterTimedAlgorithm::calculate()
{
	return mFilter->execute();
}


} // namespace cx
