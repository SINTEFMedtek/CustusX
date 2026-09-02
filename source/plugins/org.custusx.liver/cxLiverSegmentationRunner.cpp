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
#include "cxDataLocations.h"
#include "cxImage.h"
#include "cxVisServices.h"
#include "cxSelectDataStringPropertyBase.h"
#include "cxLogger.h"

namespace cx
{

LiverSegmentationRunner::LiverSegmentationRunner(VisServicesPtr services, QObject* parent) :
	QObject(parent),
	mServices(services)
{
}

LiverSegmentationRunner::~LiverSegmentationRunner()
{
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
	connect(mCurrentFilter.get(), &GenericScriptFilter::scriptOutput,
	        this, &LiverSegmentationRunner::onScriptOutput,
	        Qt::ConnectionType(Qt::QueuedConnection | Qt::UniqueConnection));

	emit filterStarted(iniFileName);
	mCurrentThread->execute();
}

void LiverSegmentationRunner::onFilterFinished()
{
	disconnect(mCurrentThread.get(), &FilterTimedAlgorithm::finished, this, &LiverSegmentationRunner::onFilterFinished);
	disconnect(mCurrentFilter.get(), &GenericScriptFilter::scriptOutput, this, &LiverSegmentationRunner::onScriptOutput);
	mCurrentThread.reset();
	mCurrentFilter.reset();

	this->runNext();
}

void LiverSegmentationRunner::onScriptOutput(const QString& line)
{
	if (!line.startsWith("PROGRESS:"))
		return;

	bool ok = false;
	int percent = line.mid(9).trimmed().toInt(&ok);
	if (ok)
		emit progressChanged(percent);
}

} /* namespace cx */
