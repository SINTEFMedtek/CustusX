/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxNdiTrackerThread.h"

namespace cx
{

NdiTrackerThread::NdiTrackerThread(ToolFileParser::TrackerInternalStructure trackerStructure,
								   std::vector<ToolFileParser::ToolInternalStructurePtr> toolStructures,
								   ToolFileParser::ToolInternalStructurePtr referenceToolStructure)
{
	this->setObjectName("org.custusx.core.tracking.ndi"); // becomes the thread name
	mInitTrackerStructure = trackerStructure;
	mInitToolStructures = toolStructures;
	mInitReferenceToolStructure = referenceToolStructure;
}

NdiTrackerThread::~NdiTrackerThread()
{
}

std::map<QString, NdiToolPtr> NdiTrackerThread::getTools()
{
	return mManager->getTools();
}

NdiToolPtr NdiTrackerThread::getReferenceTool()
{
	return mManager->getReferenceTool();
}

void NdiTrackerThread::initialize(bool on)
{
	emit requestInitialize(on);
}

void NdiTrackerThread::track(bool on)
{
	emit requestTrack(on);
}

void NdiTrackerThread::setConnectionSettings(const NdiConnectionSettings& connection)
{
	if (mManager)
	{
		mManager->setConnectionSettings(connection);
	}
}

void NdiTrackerThread::run()
{
	this->configure();
	this->exec(); // run event loop
	this->deconfigure();
}

void NdiTrackerThread::configure()
{
	mManager.reset(new NdiTrackerManager(
		mInitTrackerStructure,
		mInitToolStructures,
		mInitReferenceToolStructure));

	connect(mManager.get(), &NdiTrackerManager::initialized, this, &NdiTrackerThread::initialized);
	connect(mManager.get(), &NdiTrackerManager::tracking, this, &NdiTrackerThread::tracking);
	connect(mManager.get(), &NdiTrackerManager::error, this, &NdiTrackerThread::error);
	connect(this, &NdiTrackerThread::requestInitialize, mManager.get(), &NdiTrackerManager::initializeSlot);
	connect(this, &NdiTrackerThread::requestTrack, mManager.get(), &NdiTrackerManager::trackSlot);

	emit configured(true);
}

void NdiTrackerThread::deconfigure()
{
	QObject::disconnect(mManager.get());
	mManager.reset();
	emit configured(false);
}

} // namespace cx
