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


#ifndef CXTOOLMANAGER_H_
#define CXTOOLMANAGER_H_

#include "cxResourceExport.h"

#include <map>
#include <vector>
#include <boost/shared_ptr.hpp>
#include <QObject>
#include <QDomNode>
#include "cxTransform3D.h"
#include "cxTool.h"
#include "cxLandmark.h"

class QDomNode;

namespace cx
{

typedef std::map<ToolPtr, TimedTransformMap> SessionToolHistoryMap;
typedef boost::shared_ptr<class Landmarks> LandmarksPtr;
typedef boost::shared_ptr<class PlaybackTime> PlaybackTimePtr;
typedef boost::shared_ptr<class TrackerConfiguration> TrackerConfigurationPtr;


/**\brief Manager interface for tools and tracking systems.
 *
 *
 * \ingroup cx_resource_core_tool
 */
class cxResource_EXPORT ToolManager: public QObject
{
Q_OBJECT
public:
	typedef std::map<QString, ToolPtr> ToolMap;

	virtual Tool::State getState() const = 0;
	virtual void setState(const Tool::State val) = 0;

	virtual ToolMap getTools() = 0; ///< get configured and initialized tools
	virtual ToolPtr getTool(const QString& uid) = 0; ///< get a tool

	virtual void setTooltipOffset(double) {}; ///< set the tool tip offset
	virtual double getTooltipOffset() const {return 0.0;}; ///< get the tool tip offset

	virtual ToolPtr getDominantTool() = 0; ///< get the tool that has higest priority when tracking
	virtual void setDominantTool(const QString& uid) = 0; ///< set a tool to be the dominant tool
	virtual void dominantCheckSlot() = 0; ///< checks if the visible tool is going to be set as dominant tool
	virtual ToolPtr findFirstProbe() = 0;

	virtual ToolPtr getReferenceTool() const = 0; ///< tool used as patient reference
	virtual ManualToolPtr getManualTool() = 0; ///< a mouse-controllable virtual tool that is available even when not tracking.

	virtual bool isPlaybackMode() const { return false; }
	virtual void setPlaybackMode(PlaybackTimePtr controller) {}

	virtual void savePositionHistory() = 0;
	virtual void loadPositionHistory() = 0;
	virtual void addXml(QDomNode& parentNode) = 0;
	virtual void parseXml(QDomNode& dataNode) = 0;
	virtual void clear() = 0;
	virtual SessionToolHistoryMap getSessionHistory(double startTime, double stopTime) = 0;
	virtual void setLoggingFolder(QString loggingFolder) {}
	virtual void runDummyTool(DummyToolPtr tool) {}
	virtual QStringList getSupportedTrackingSystems() { return QStringList(); }
	virtual TrackerConfigurationPtr getConfiguration() = 0;

//public slots:
//	virtual void saveToolsSlot() {}

signals:
	void stateChanged();
	void dominantToolChanged(const QString& uId);
//	void tps(int); ///< the dominant tools tps
	void tooltipOffset(double offset);

protected:
	virtual ~ToolManager() {}

};

} //namespace cx
#endif /* CXTOOLMANAGER_H_ */
