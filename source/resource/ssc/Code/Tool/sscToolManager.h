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

#ifndef SSCTOOLMANAGER_H_
#define SSCTOOLMANAGER_H_

#include <map>
#include <vector>
#include <boost/shared_ptr.hpp>
#include <QObject>
#include <QDomNode>
#include "sscTransform3D.h"
#include "sscTool.h"
#include "sscLandmark.h"

class QDomNode;

namespace ssc
{

typedef std::map<ToolPtr, TimedTransformMap> SessionToolHistoryMap;
typedef boost::shared_ptr<class RegistrationHistory> RegistrationHistoryPtr;

/**\brief Manager interface for tools and tracking systems.
 *
 * A singleton for managing all tools.
 * Configuration, active tool, patient registration.
 *
 * Implementations of ToolManager typically connect to
 * a physical tracking system.
 *
 * \ingroup sscTool
 */
class ToolManager: public QObject
{
Q_OBJECT
public:
	typedef std::map<QString, ToolPtr> ToolMap;
	typedef boost::shared_ptr<ToolMap> ToolMapPtr;

	/** not sure if this is needed? we have getInstance in subclasses...*/
	static void setInstance(ToolManager* instance); ///< must call this one before calling getInstance()
	static ToolManager* getInstance();
	static void shutdown();

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

	virtual std::map<QString, QString> getToolUidsAndNames() const = 0; ///< get all tools uids and names
	virtual std::vector<QString> getToolNames() const = 0; ///< get the name of all tools
	virtual std::vector<QString> getToolUids() const = 0; ///< get the uid of all the tools

	virtual Transform3DPtr get_rMpr() const = 0; ///< transform from patient ref to ref space
	virtual void set_rMpr(const Transform3DPtr& val) = 0; ///< set transform from patient ref to ref space
	virtual RegistrationHistoryPtr get_rMpr_History()
	{
		return RegistrationHistoryPtr();
	} ///< interface to rMpr history.
	virtual ToolPtr getReferenceTool() const = 0; ///< tool used as patient reference

	virtual void savePositionHistory()
	{
	}
	virtual void loadPositionHistory()
	{
	}

	virtual void addXml(QDomNode& parentNode)
	{
		Q_UNUSED(parentNode);
	} ///< write internal state to node
	virtual void parseXml(QDomNode& dataNode)
	{
		Q_UNUSED(dataNode);
	} ///< read internal state from node
	virtual void clear()
	{
	} ///< clear everything loaded from xml

	virtual ssc::LandmarkMap getLandmarks()
	{
		return ssc::LandmarkMap();
	}
	virtual void setLandmark(ssc::Landmark landmark)
	{
		Q_UNUSED(landmark);
	}
	virtual void removeLandmark(QString uid)
	{
		Q_UNUSED(uid);
	}
	virtual void removeLandmarks()
	{
	}
	;

	virtual SessionToolHistoryMap getSessionHistory(double startTime, double stopTime)
	{
		Q_UNUSED(startTime);
		Q_UNUSED(stopTime);
		return SessionToolHistoryMap();
	}
	;

signals:
	void configured(); ///< system is configured
	void deconfigured(); ///<
	void initialized(); ///< system is initialized
	void uninitialized(); ///< system is uninitialized
	void trackingStarted(); ///< system starts tracking
	void trackingStopped(); ///< system stops tracking

	void dominantToolChanged(const QString& uId); ///<signal for change of dominant tool
	void landmarkRemoved(QString uid);
	void landmarkAdded(QString uid);
	void rMprChanged(); ///< emitted when the transformation between patient reference and (data) reference is set
	void tps(int); ///< the dominant tools tps
	void tooltipOffset(double offset); ///< The tool tip offset

protected:
	ToolManager()
	{
	} ///< Empty on purpose
	~ToolManager()
	{
	} ///< Empty on purpose

	static ToolManager* mInstance; ///< The only instance of this class that can exist.
};

/**Shortcut for accessing the toolmanager instance.*/
ToolManager* toolManager();

} //namespace ssc
#endif /* SSCTOOLMANAGER_H_ */
