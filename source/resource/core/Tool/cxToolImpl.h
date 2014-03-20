// This file is part of CustusX, an Image Guided Therapy Application.
//
// Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
//
// CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
// code and binaries can only be used by SMT and those with explicit permission
// from SMT. CustusX shall not be distributed to anyone else.
//
// CustusX is a research tool. It is NOT intended for use or certified for use
// in a normal clinical setting. SMT does not take responsibility for its use
// in any way.
//
// See CustusX_License.txt for more information.
#ifndef CXTOOLIMPL_H
#define CXTOOLIMPL_H

#include "cxTool.h"

namespace cx
{
class ToolManager;
typedef boost::shared_ptr<class TrackingPositionFilter> TrackingPositionFilterPtr;

/** \brief Common functionality for Tool subclasses
 *
 *
 * \ingroup cx_resource_core_tool
 * \date 2014-02-21
 * \author christiana
 */
class ToolImpl : public Tool
{
	Q_OBJECT
public:
	explicit ToolImpl(TrackingServicePtr manager, const QString& uid="", const QString& name ="");
	virtual ~ToolImpl();

	virtual TimedTransformMapPtr getPositionHistory();
	virtual TimedTransformMap getSessionHistory(double startTime, double stopTime);
	virtual Transform3D get_prMt() const;

	virtual double getTooltipOffset() const;
	virtual void setTooltipOffset(double val);

	void resetTrackingPositionFilter(TrackingPositionFilterPtr filter);

protected:
	TrackingServicePtr getTrackingService();
	TrackingServicePtr getTrackingService() const;
	virtual void set_prMt(const Transform3D& prMt, double timestamp);
	TimedTransformMapPtr mPositionHistory;
	TrackingServiceWeakPtr mManager;
	Transform3D m_prMt; ///< the transform from the tool to the patient reference
	TrackingPositionFilterPtr mTrackingPositionFilter;
private slots:
private:
};


} // namespace cx

#endif // CXTOOLIMPL_H
