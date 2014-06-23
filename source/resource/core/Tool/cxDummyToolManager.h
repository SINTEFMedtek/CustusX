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

#ifndef CXDUMMYTOOLMANAGER_H_
#define CXDUMMYTOOLMANAGER_H_

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
class DummyToolManager : public ToolManager
{
	Q_OBJECT

public:
	typedef boost::shared_ptr<DummyToolManager> DummyToolManagerPtr;
	static DummyToolManagerPtr create();

	virtual ~DummyToolManager();

	typedef std::map<QString, DummyToolPtr> DummyToolMap;

	virtual bool isConfigured() const;
	virtual bool isInitialized() const;
	virtual bool isTracking() const;

	virtual void configure();
	virtual void deconfigure() {}
	virtual void initialize();
	virtual void uninitialize();
	virtual void startTracking();
	virtual void stopTracking();

	virtual ToolMap getTools();
	virtual ToolPtr getTool(const QString& uid);

	virtual ToolPtr getDominantTool();
	virtual void setDominantTool(const QString& uid);
	virtual void dominantCheckSlot() {}

	virtual Transform3D get_rMpr() const;
	virtual void set_rMpr(const Transform3D& val);
	virtual ToolPtr getReferenceTool() const;

	virtual void setTooltipOffset(double offset);
	virtual double getTooltipOffset() const;

	virtual ManualToolPtr getManualTool() { return ManualToolPtr(); }
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

	Transform3D m_rMpr;
	double mToolTipOffset; ///< Common tool tip offset for all tools

	bool mConfigured;
	bool mInitialized;
	bool mIsTracking;
};

}//namespace cx

#endif /* CXDUMMYTOOLMANAGER_H_ */
