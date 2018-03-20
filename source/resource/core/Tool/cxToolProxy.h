/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef CXTOOLPROXY_H
#define CXTOOLPROXY_H

#include "cxTool.h"
#include "cxServiceTrackerListener.h"

class ctkPluginContext;

namespace cx
{
typedef boost::shared_ptr<class TrackingService> TrackingServicePtr;

/** Proxy for a real Tool. Turns to Null when Tool becomes unavailable
 *
 * \ingroup cx_resource_core_tool
 */
class cxResource_EXPORT ToolProxy: public Tool
{
	Q_OBJECT
public:
	static ToolPtr create(ToolPtr base, ctkPluginContext *pluginContext);

	virtual ~ToolProxy() {}

	virtual std::set<Type> getTypes() const;
	virtual vtkPolyDataPtr getGraphicsPolyData() const;
	virtual TimedTransformMapPtr getPositionHistory();
	virtual ToolPositionMetadata getMetadata() const;
	virtual const std::map<double, ToolPositionMetadata>& getMetadataHistory();

	virtual bool getVisible() const;
	virtual bool isInitialized() const;

	virtual QString getUid() const;
	virtual QString getName() const;

	virtual bool isCalibrated() const;
	virtual Transform3D getCalibration_sMt() const;
	virtual void setCalibration_sMt(Transform3D calibration);

	virtual ProbePtr getProbe() const;
	virtual double getTimestamp() const;
	virtual void printSelf(std::ostream &os, Indent indent);

	virtual double getTooltipOffset() const;
	virtual void setTooltipOffset(double val);
	virtual std::map<int, Vector3D> getReferencePoints() const;
	virtual bool hasReferencePointWithId(int id);

	virtual TimedTransformMap getSessionHistory(double startTime, double stopTime);
	virtual Transform3D get_prMt() const;

	virtual void resetTrackingPositionFilter(TrackingPositionFilterPtr filter);

	virtual void set_prMt(const Transform3D& prMt, double timestamp);
	virtual void setVisible(bool vis);

	virtual bool isNull();

private slots:
	void onStateChanged();
private:
	ToolProxy(ToolPtr base, ctkPluginContext *pluginContext);
	void initServiceListener();
	void onServiceAdded(TrackingService* service);
	void onServiceRemoved(TrackingService *service);
	void checkToolValidity();

	ctkPluginContext *mPluginContext;
	ToolPtr mTool;
	TrackingServicePtr mTrackingService;
	boost::shared_ptr<ServiceTrackerListener<TrackingService> > mServiceListener;

};

} // namespace cx

#endif // CXTOOLPROXY_H
