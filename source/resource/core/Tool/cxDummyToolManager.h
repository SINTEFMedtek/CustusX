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


#ifndef CXDUMMYTOOLMANAGER_H_
#define CXDUMMYTOOLMANAGER_H_

#include "cxResourceExport.h"

#include "cxToolManager.h"
#include "cxDummyTool.h"

namespace cx
{

/**\brief Implementation of a ToolManager used for testing.
 *
 * \date Oct 29, 2008
 * \author: jbake
 *
 * \ingroup cx_resource_core_tool
 */
class cxResource_EXPORT DummyToolManager : public ToolManager
{
	Q_OBJECT

public:
	typedef boost::shared_ptr<DummyToolManager> DummyToolManagerPtr;
	static DummyToolManagerPtr create();

	virtual ~DummyToolManager();

	typedef std::map<QString, DummyToolPtr> DummyToolMap;

	virtual Tool::State getState() const;
	virtual void setState(const Tool::State val);

	virtual ToolMap getTools();
	virtual ToolPtr getTool(const QString& uid);

	virtual ToolPtr getActiveTool();
	virtual void setActiveTool(const QString& uid);

	virtual Transform3D get_rMpr() const;
	virtual void set_rMpr(const Transform3D& val);
	virtual ToolPtr getReferenceTool() const;

	virtual ToolPtr getManualTool() { return ToolPtr(); }
	virtual void savePositionHistory() {}
	virtual void loadPositionHistory() {}
	virtual void addXml(QDomNode& parentNode) {}
	virtual void parseXml(QDomNode& dataNode) {}
	virtual void clear() {}
	virtual SessionToolHistoryMap getSessionHistory(double startTime, double stopTime) { return SessionToolHistoryMap(); }

	void addTool(DummyToolPtr tool);
	virtual ToolPtr findFirstProbe() { return ToolPtr(); }
	virtual TrackerConfigurationPtr getConfiguration() { return TrackerConfigurationPtr(); }

private:
	typedef DummyToolMap::iterator DummyToolMapIter;
	typedef DummyToolMap::const_iterator DummyToolMapConstIter;

	DummyToolManager();
	TrackingServiceWeakPtr mSelf;

	DummyToolMap mDummyTools;
	DummyToolPtr mDominantTool;
	DummyToolPtr mReferenceTool;

	virtual void startTracking();
	virtual void stopTracking();

	Transform3D m_rMpr;
	double mToolTipOffset; ///< Common tool tip offset for all tools

//	bool mConfigured;
//	bool mInitialized;
//	bool mIsTracking;
	Tool::State mState;
};

}//namespace cx

#endif /* CXDUMMYTOOLMANAGER_H_ */
