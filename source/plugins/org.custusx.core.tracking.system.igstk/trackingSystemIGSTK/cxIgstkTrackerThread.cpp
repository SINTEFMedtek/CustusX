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

#include "cxIgstkTrackerThread.h"

#include "cxIgstkTool.h"
#include "cxIgstkToolManager.h"


namespace cx
{

IgstkTrackerThread::IgstkTrackerThread(ToolFileParser::TrackerInternalStructure trackerStructure,
								std::vector<ToolFileParser::ToolInternalStructurePtr> toolStructures,
								ToolFileParser::ToolInternalStructurePtr referenceToolStructure)
{
	this->setObjectName("org.custusx.core.tracking.igstk"); // becomes the thread name
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

	emit configured(true);
}

void IgstkTrackerThread::deconfigure()
{
	QObject::disconnect(mManager.get());
	mManager.reset();

	emit configured(false);
}

} //namespace cx
