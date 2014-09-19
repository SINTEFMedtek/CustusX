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


#include "cxDummyToolManager.h"

namespace cx
{

//TrackingServiceOldPtr ToolManager::mInstance = NULL;

DummyToolManager::DummyToolManagerPtr DummyToolManager::create()
{
	DummyToolManagerPtr retval;
	retval.reset(new DummyToolManager());
	retval->mSelf = retval;
	return retval;
}

//TrackingServiceOldPtr DummyToolManager::getInstance()
//{
//	if(ToolManager::mInstance == NULL)
//	{
//		ToolManager::mInstance = new DummyToolManager();
//	}
//	return ToolManager::mInstance;
//}

//void DummyToolManager::reset()
//{
//	ToolManager::mInstance = NULL;
//	getInstance();
//}

DummyToolManager::DummyToolManager() :
	m_rMpr(Transform3D::Identity()),
	mToolTipOffset(0),
	mConfigured(false),
	mInitialized(false),
	mIsTracking(false)
{
	DummyToolPtr tool1(new DummyTool(mSelf.lock()));

	mDominantTool = tool1;
	mReferenceTool = tool1;

	mDummyTools.insert(std::pair<QString, DummyToolPtr>(tool1->getUid(), tool1));
}
DummyToolManager::~ DummyToolManager()
{}
bool DummyToolManager::isConfigured() const
{
	return mConfigured;
}
bool DummyToolManager::isInitialized() const
{
	return mInitialized;
}
bool DummyToolManager::isTracking() const
{
	return mIsTracking;
}

void DummyToolManager::configure()
{
	mConfigured = true;
	emit configured();
}
void DummyToolManager::initialize()
{
	mInitialized = true;
	emit initialized();
}
void DummyToolManager::uninitialize()
{
	mInitialized = false;
	emit initialized();
}

void DummyToolManager::startTracking()
{
	mIsTracking = true;
	emit trackingStarted();

	DummyToolMapConstIter it = mDummyTools.begin();
	while(it != mDummyTools.end())
	{
		((*it).second)->startTracking();
		++it;
	}

}

void DummyToolManager::stopTracking()
{
	mIsTracking = false;
	emit trackingStopped();

	DummyToolMapConstIter it = mDummyTools.begin();
	while(it != mDummyTools.end())
	{
		((*it).second)->stopTracking();
		++it;
	}
}

ToolManager::ToolMap DummyToolManager::getTools()
{
	return ToolManager::ToolMap(mDummyTools.begin(), mDummyTools.end());
}

ToolPtr DummyToolManager::getTool(const QString& uid)
{
	DummyToolMapConstIter it = mDummyTools.find(uid);
	return (*it).second;
}

ToolPtr DummyToolManager::getDominantTool()
{
	return mDominantTool;
}
void DummyToolManager::setDominantTool(const QString& uid)
{
	DummyToolMapConstIter it = mDummyTools.find(uid);
	mDominantTool = (*it).second;
	emit dominantToolChanged(uid);
}

Transform3D DummyToolManager::get_rMpr() const
{
	return m_rMpr;
}

void DummyToolManager::set_rMpr(const Transform3D& val)
{
	m_rMpr = val;
}

ToolPtr DummyToolManager::getReferenceTool() const
{
	return mReferenceTool;
}

void DummyToolManager::addTool(DummyToolPtr tool)
{
	mDummyTools.insert(std::make_pair(tool->getUid(), tool));
}

void DummyToolManager::setTooltipOffset(double offset)
{
	if (similar(offset, mToolTipOffset))
		return;
	mToolTipOffset = offset;
	emit tooltipOffset(mToolTipOffset);
}
double DummyToolManager::getTooltipOffset() const
{
	return mToolTipOffset;
}


} //namespace cx
