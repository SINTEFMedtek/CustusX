/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxLiverSegmentationRunner.h"

#include <QApplication>

#include "cxGenericScriptFilter.h"
#include "cxFilterTimedAlgorithm.h"
#include "cxDataLocations.h"
#include "cxImage.h"
#include "cxVisServices.h"
#include "cxSelectDataStringPropertyBase.h"
#include "cxLogger.h"

namespace cx
{

QString QueuedRun::key() const
{
	return iniFileName + "@" + image->getUid();
}

LiverSegmentationRunner::LiverSegmentationRunner(VisServicesPtr services, QObject* parent) :
	QObject(parent),
	mServices(services)
{
	// LogicManager's own aboutToQuit-triggered shutdown is disabled (see
	// cxLogicManager.cpp) to avoid CTK-related crashes, so a running filter
	// isn't otherwise given a chance to stop its child process on app exit.
	connect(qApp, &QApplication::aboutToQuit, this, &LiverSegmentationRunner::stop);
}

LiverSegmentationRunner::~LiverSegmentationRunner()
{
}

bool LiverSegmentationRunner::isRunning() const
{
	return mCurrentThread != nullptr;
}

void LiverSegmentationRunner::start(QList<QueuedRun> queue)
{
	if (this->isRunning())
	{
		reportWarning("LiverSegmentationRunner::start: A segmentation is already running.");
		return;
	}
	mQueue = queue;
	this->runNext();
}

GenericScriptFilterPtr LiverSegmentationRunner::createFilter(const QueuedRun& run)
{
	GenericScriptFilterPtr filter(new GenericScriptFilter(mServices));
	std::vector<SelectDataStringPropertyBasePtr> input = filter->getInputTypes();
	filter->getOutputTypes();
	filter->getOptions();
	filter->setParameterFilePath(DataLocations::getFilterScriptsPath() + run.iniFileName);
	if (!input.empty())
		input[0]->setValue(run.image->getUid());
	return filter;
}

void LiverSegmentationRunner::runNext()
{
	if (mQueue.isEmpty())
	{
		emit allFinished();
		return;
	}
	QueuedRun run = mQueue.takeFirst();
	mCurrentFilter = this->createFilter(run);

	mCurrentThread.reset(new FilterTimedAlgorithm(mCurrentFilter));
	connect(mCurrentThread.get(), &FilterTimedAlgorithm::finished, this, &LiverSegmentationRunner::onFilterFinished);
	connect(mCurrentFilter.get(), &GenericScriptFilter::scriptOutput,
	        this, &LiverSegmentationRunner::onScriptOutput,
	        Qt::ConnectionType(Qt::QueuedConnection | Qt::UniqueConnection));

	emit filterStarted(run.key());
	mCurrentThread->execute();
}

void LiverSegmentationRunner::stop()
{
	mQueue.clear();
	if (mCurrentFilter)
		mCurrentFilter->requestStop();
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
