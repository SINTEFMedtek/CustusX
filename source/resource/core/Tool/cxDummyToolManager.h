/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/


#ifndef CXDUMMYTOOLMANAGER_H_
#define CXDUMMYTOOLMANAGER_H_

#include "cxResourceExport.h"

#include "cxTrackingService.h"
#include "cxDummyTool.h"

namespace cx
{

/** \brief Implementation of a ToolManager used for testing.
 *
 * \date Oct 29, 2008
 * \author: jbake
 *
 * \ingroup cx_resource_core_tool
 */
class cxResource_EXPORT DummyToolManager : public TrackingService
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
	virtual void clearActiveTool();

	virtual Transform3D get_rMpr() const;
	virtual void set_rMpr(const Transform3D& val);
	virtual ToolPtr getReferenceTool() const;

	virtual ToolPtr getManualTool() { return ToolPtr(); }
//	virtual void savePositionHistory() {}
//	virtual void loadPositionHistory() {}
//	virtual void addXml(QDomNode& parentNode) {}
//	virtual void parseXml(QDomNode& dataNode) {}
//	virtual void clear() {}
	virtual SessionToolHistoryMap getSessionHistory(double startTime, double stopTime) { return SessionToolHistoryMap(); }

	virtual bool isPlaybackMode() const { return false; }
	virtual void setPlaybackMode(PlaybackTimePtr controller) {}
//	virtual void setLoggingFolder(QString loggingFolder) {}
	virtual void runDummyTool(DummyToolPtr tool) {}
	virtual bool isNull() { return false; }

	void addTool(DummyToolPtr tool);
	virtual ToolPtr getFirstProbe() { return ToolPtr(); }
	virtual std::vector<TrackerConfigurationPtr> getConfigurations();
	//virtual TrackerConfigurationPtr getConfiguration(QString trackingSystemImplementation);
	virtual TrackerConfigurationPtr getConfiguration();
	virtual void setCurrentTrackingSystemImplementation(QString trackingSystemImplementation);
	virtual QString getCurrentTrackingSystemImplementation();

	virtual void installTrackingSystem(TrackingSystemServicePtr system);
	virtual void unInstallTrackingSystem(TrackingSystemServicePtr system);
	virtual std::vector<TrackingSystemServicePtr> getTrackingSystems();
	virtual void resetTimeSynchronization();

private:
	typedef DummyToolMap::iterator DummyToolMapIter;
	typedef DummyToolMap::const_iterator DummyToolMapConstIter;

	DummyToolManager();

	DummyToolMap mDummyTools;
	DummyToolPtr mActiveTool;
	DummyToolPtr mReferenceTool;

	virtual void startTracking();
	virtual void stopTracking();

	Transform3D m_rMpr;
//	double mToolTipOffset; ///< Common tool tip offset for all tools

//	bool mConfigured;
//	bool mInitialized;
//	bool mIsTracking;
	Tool::State mState;
};

}//namespace cx

#endif /* CXDUMMYTOOLMANAGER_H_ */
