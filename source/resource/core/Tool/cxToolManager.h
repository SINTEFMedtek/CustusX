// This file is part of SSC,
// a C++ Library supporting Image Guided Therapy Applications.
//
// Copyright (C) 2008- SINTEF Medical Technology
// Copyright (C) 2008- Sonowand AS
//
// SSC is owned by SINTEF Medical Technology and Sonowand AS,
// hereafter named the owners. Each particular piece of code
// is owned by the part that added it to the library.
// SSC source code and binaries can only be used by the owners
// and those with explicit permission from the owners.
// SSC shall not be distributed to anyone else.
//
// SSC is distributed WITHOUT ANY WARRANTY; without even
// the implied warranty of MERCHANTABILITY or FITNESS FOR
// A PARTICULAR PURPOSE.
//
// See sscLicense.txt for more information.

#ifndef CXTOOLMANAGER_H_
#define CXTOOLMANAGER_H_

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

/**\brief Manager interface for tools and tracking systems.
 *
 * A singleton for managing all tools.
 * Configuration, active tool.
 *
 * Implementations of ToolManager typically connect to
 * a physical tracking system.
 *
 * \ingroup cx_resource_core_tool
 */
class ToolManager: public QObject
{
Q_OBJECT
public:
	typedef std::map<QString, ToolPtr> ToolMap;
	typedef boost::shared_ptr<ToolMap> ToolMapPtr;

//	/** not sure if this is needed? we have getInstance in subclasses...*/
//	static void setInstance(TrackingServicePtr instance); ///< must call this one before calling getInstance()
//	static TrackingServicePtr getInstance();
//	static void shutdown();

	virtual bool isConfigured() const = 0; ///< system is ready to use but not connected to hardware
	virtual bool isInitialized() const = 0; ///< system is connected to hw and ready
	virtual bool isTracking() const = 0; ///< system is tracking

	virtual void configure() = 0; ///< get the system ready without connecting to the tracking hardware
	virtual void initialize() = 0; ///< connect the system to the tracking hardware
	virtual void uninitialize() = 0; ///< disconnect the system from the hardware
	virtual void startTracking() = 0; ///< start tracking
	virtual void stopTracking() = 0; ///< stop tracking

	virtual ToolMapPtr getConfiguredTools() = 0; ///< get all configured tools
	virtual ToolMapPtr getInitializedTools() = 0; ///< get all initialized tools
	virtual ToolMapPtr getTools() = 0; ///< get configured and initialized tools
	virtual ToolPtr getTool(const QString& uid) = 0; ///< get a tool
	virtual void setTooltipOffset(double) {}; ///< set the tool tip offset
	virtual double getTooltipOffset() const {return 0.0;}; ///< get the tool tip offset

	virtual ToolPtr getDominantTool() = 0; ///< get the tool that has higest priority when tracking
	virtual void setDominantTool(const QString& uid) = 0; ///< set a tool to be the dominant tool
	virtual void dominantCheckSlot() = 0; ///< checks if the visible tool is going to be set as dominant tool

	virtual std::map<QString, QString> getToolUidsAndNames() const = 0; ///< get all tools uids and names
	virtual std::vector<QString> getToolNames() const = 0; ///< get the name of all tools
	virtual std::vector<QString> getToolUids() const = 0; ///< get the uid of all the tools

	virtual ToolPtr getReferenceTool() const = 0; ///< tool used as patient reference
	virtual ManualToolPtr getManualTool() = 0; ///< a mouse-controllable virtual tool that is available even when not tracking.
	virtual void savePositionHistory() = 0;
	virtual void loadPositionHistory() = 0;
	virtual void addXml(QDomNode& parentNode) = 0;
	virtual void parseXml(QDomNode& dataNode) = 0;
	virtual void clear() = 0;
	virtual SessionToolHistoryMap getSessionHistory(double startTime, double stopTime) = 0;
	virtual ToolPtr findFirstProbe() = 0;
	virtual void setLoggingFolder(QString loggingFolder) {}

	virtual bool isPlaybackMode() const { return false; }
	virtual void setPlaybackMode(PlaybackTimePtr controller) {}

	virtual void runDummyTool(DummyToolPtr tool) {}
	virtual QStringList getSupportedTrackingSystems() { return QStringList(); }

public slots:
	virtual void deconfigure() {}
	virtual void saveToolsSlot() {}

signals:
	void configured(); ///< system is configured
	void deconfigured(); ///<
	void initialized(); ///< system is initialized
	void uninitialized(); ///< system is uninitialized
	void trackingStarted(); ///< system starts tracking
	void trackingStopped(); ///< system stops tracking

	void dominantToolChanged(const QString& uId); ///<signal for change of dominant tool
	void tps(int); ///< the dominant tools tps
	void tooltipOffset(double offset); ///< The tool tip offset

protected:
	ToolManager()
	{
	} ///< Empty on purpose
	~ToolManager()
	{
	} ///< Empty on purpose

//	static TrackingServicePtr mInstance; ///< The only instance of this class that can exist.
};

/**Shortcut for accessing the toolmanager instance.*/
//TrackingServicePtr toolManager();

} //namespace cx
#endif /* CXTOOLMANAGER_H_ */
