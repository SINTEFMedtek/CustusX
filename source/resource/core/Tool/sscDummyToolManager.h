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

#ifndef SSCDUMMYTOOLMANAGER_H_
#define SSCDUMMYTOOLMANAGER_H_

#include "sscToolManager.h"
#include "sscDummyTool.h"

namespace cx
{

/**\brief Implementation of a ToolManager used for testing.
 *
 * \date Oct 29, 2008
 * \author: jbake
 *
 * \ingroup sscTool
 */
class DummyToolManager : public ToolManager
{
	Q_OBJECT

public:
	typedef boost::shared_ptr<DummyToolManager> DummyToolManagerPtr;
	static DummyToolManagerPtr create();

	virtual ~DummyToolManager();

	typedef std::map<QString, DummyToolPtr> DummyToolMap;
	typedef boost::shared_ptr<DummyToolMap> DummyToolMapPtr;

//	static TrackingServicePtr getInstance();

	virtual bool isConfigured() const;
	virtual bool isInitialized() const;
	virtual bool isTracking() const;

	virtual void configure();
	virtual void initialize();
	virtual void uninitialize();
	virtual void startTracking();
	virtual void stopTracking();

	virtual ToolMapPtr getConfiguredTools();
	virtual ToolMapPtr getInitializedTools();
	virtual ToolMapPtr getTools();
	virtual ToolPtr getTool(const QString& uid);

	virtual ToolPtr getDominantTool();
	virtual void setDominantTool(const QString& uid);
	virtual void dominantCheckSlot() {}

	virtual std::map<QString, QString> getToolUidsAndNames() const;
	virtual std::vector<QString> getToolNames() const;
	virtual std::vector<QString> getToolUids() const;

	virtual Transform3D get_rMpr() const;
	virtual void set_rMpr(const Transform3D& val);
	virtual ToolPtr getReferenceTool() const;
	virtual void saveTransformsAndTimestamps(QString filePathAndName = "");

	virtual void setTooltipOffset(double offset);
	virtual double getTooltipOffset() const;

	virtual LandmarksPtr getPatientLandmarks() { return Landmarks::create(); }

	virtual ManualToolPtr getManualTool() { return ManualToolPtr(); }
	virtual void savePositionHistory() {}
	virtual void loadPositionHistory() {}
	virtual void addXml(QDomNode& parentNode) {}
	virtual void parseXml(QDomNode& dataNode) {}
	virtual void clear() {}
	virtual SessionToolHistoryMap getSessionHistory(double startTime, double stopTime) { return SessionToolHistoryMap(); }

//	static DummyToolManager* getDowncastInstance();
	/**
	 * Reset all internal state of the DummyToolMananger instance. Call between tests to avoid state leak.
	 */
//	static void reset();
	void addTool(DummyToolPtr tool);
	virtual ToolPtr findFirstProbe() { return ToolPtr(); }


private:
	typedef DummyToolMap::iterator DummyToolMapIter;
	typedef DummyToolMap::const_iterator DummyToolMapConstIter;

	DummyToolManager();
	TrackingServiceWeakPtr mSelf;

	DummyToolMapPtr mDummyTools;
	DummyToolPtr mDominantTool;
	DummyToolPtr mReferenceTool;

	Transform3D m_rMpr;
	double mToolTipOffset; ///< Common tool tip offset for all tools

	bool mConfigured;
	bool mInitialized;
	bool mIsTracking;
};

}//namespace cx

#endif /* SSCDUMMYTOOLMANAGER_H_ */
