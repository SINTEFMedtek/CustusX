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

#include <cxIgstkTrackerThread.h>
#include "sscMessageManager.h"
#include "cxIgstkTool.h"
#include <cxIgstkToolManager.h>

namespace cx
{

IgstkTrackerThread::IgstkTrackerThread(IgstkTracker::InternalStructure trackerStructure,
				std::vector<IgstkTool::InternalStructure> toolStructures,
				IgstkTool::InternalStructure referenceToolStructure)
{
	mInitTrackerStructure = trackerStructure;
	mInitToolStructures = toolStructures;
	mInitReferenceToolStructure = referenceToolStructure;
}

IgstkTrackerThread::~IgstkTrackerThread()
{
}

std::map<QString, IgstkToolPtr> IgstkTrackerThread::getTools()
{
	return mManager->getTools();
}

IgstkToolPtr IgstkTrackerThread::getRefereceTool()
{
	return mManager->getRefereceTool();
}

void IgstkTrackerThread::initialize(bool on)
{
	emit requestInitialize(on);
}

void IgstkTrackerThread::track(bool on)
{
	emit requestTrack(on);
}

void IgstkTrackerThread::reattachTools()
{
	emit requestReattachTools();
}

void IgstkTrackerThread::run()
{
	this->configure();

	// run event loop
	this->exec();

	this->deconfigure();
}

void IgstkTrackerThread::configure()
{
	mManager.reset(new IgstkToolManager(mInitTrackerStructure, mInitToolStructures, mInitReferenceToolStructure));
	connect(mManager.get(), SIGNAL(initialized(bool)), this, SIGNAL(initialized(bool)));
	connect(mManager.get(), SIGNAL(tracking(bool)), this, SIGNAL(tracking(bool)));
	connect(mManager.get(), SIGNAL(error()), this, SIGNAL(error()));
	connect(this, SIGNAL(requestInitialize(bool)), mManager.get(), SLOT(initializeSlot(bool)));
	connect(this, SIGNAL(requestTrack(bool)), mManager.get(), SLOT(trackSlot(bool)));
	connect(this, SIGNAL(requestReattachTools()), mManager.get(), SLOT(reattachToolsSlot(bool)));

	emit configured(true);
}

void IgstkTrackerThread::deconfigure()
{
	QObject::disconnect(mManager.get());
	mManager.reset();

	emit configured(false);
}

} //namespace cx
