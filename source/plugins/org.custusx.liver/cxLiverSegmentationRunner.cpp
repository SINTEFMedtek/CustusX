/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxLiverSegmentationRunner.h"

#include "cxGenericScriptFilter.h"
#include "cxFilterTimedAlgorithm.h"
#include "cxTimedAlgorithmProgressBar.h"
#include "cxDataLocations.h"
#include "cxImage.h"
#include "cxVisServices.h"
#include "cxSelectDataStringPropertyBase.h"
#include "cxLogger.h"

namespace cx
{

LiverSegmentationRunner::LiverSegmentationRunner(VisServicesPtr services, QObject* parent) :
	QObject(parent),
	mServices(services),
	mProgressBar(new TimedAlgorithmProgressBar())
{
}

LiverSegmentationRunner::~LiverSegmentationRunner()
{
}

TimedAlgorithmProgressBar* LiverSegmentationRunner::getProgressBar()
{
	return mProgressBar;
}

bool LiverSegmentationRunner::isRunning() const
{
	return mCurrentThread != nullptr;
}

void LiverSegmentationRunner::start(QStringList iniFileNames, ImagePtr image)
{
	if (this->isRunning())
	{
		reportWarning("LiverSegmentationRunner::start: A segmentation is already running.");
		return;
	}
	mQueue = iniFileNames;
	mImage = image;
	this->runNext();
}

GenericScriptFilterPtr LiverSegmentationRunner::createFilter(QString iniFileName)
{
	GenericScriptFilterPtr filter(new GenericScriptFilter(mServices));
	std::vector<SelectDataStringPropertyBasePtr> input = filter->getInputTypes();
	filter->getOutputTypes();
	filter->getOptions();
	filter->setParameterFilePath(DataLocations::getFilterScriptsPath() + iniFileName);
	if (!input.empty())
		input[0]->setValue(mImage->getUid());
	return filter;
}

void LiverSegmentationRunner::runNext()
{
	if (mQueue.isEmpty())
	{
		emit allFinished();
		return;
	}
	QString iniFileName = mQueue.takeFirst();
	mCurrentFilter = this->createFilter(iniFileName);

	mCurrentThread.reset(new FilterTimedAlgorithm(mCurrentFilter));
	connect(mCurrentThread.get(), &FilterTimedAlgorithm::finished, this, &LiverSegmentationRunner::onFilterFinished);
	mProgressBar->attach(mCurrentThread);

	emit filterStarted(iniFileName);
	mCurrentThread->execute();
}

void LiverSegmentationRunner::onFilterFinished()
{
	mProgressBar->detach(mCurrentThread);
	disconnect(mCurrentThread.get(), &FilterTimedAlgorithm::finished, this, &LiverSegmentationRunner::onFilterFinished);
	mCurrentThread.reset();
	mCurrentFilter.reset();

	this->runNext();
}

} /* namespace cx */
