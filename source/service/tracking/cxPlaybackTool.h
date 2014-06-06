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

#ifndef CXPLAYBACKTOOL_H_
#define CXPLAYBACKTOOL_H_

#include "cxToolImpl.h"

namespace cx
{

typedef boost::shared_ptr<class PlaybackTool> PlaybackToolPtr;
typedef boost::shared_ptr<class PlaybackTime> PlaybackTimePtr;


/** \brief A tool used during playback
 *
 * \date Mar 29, 2012
 * \author Christian Askeland, SINTEF
 *
 * \ingroup cx_service_tracking
 */
class PlaybackTool: public ToolImpl
{
Q_OBJECT
public:
	explicit PlaybackTool(TrackingServicePtr manager, ToolPtr base, PlaybackTimePtr time);
	virtual ~PlaybackTool();
	virtual std::set<Type> getTypes() const;
	virtual vtkPolyDataPtr getGraphicsPolyData() const;
	virtual void setTransformSaveFile(const QString& filename) {}
	virtual Transform3D get_prMt() const;
	virtual bool getVisible() const;
	virtual QString getUid() const;
	virtual QString getName() const;
	virtual bool isCalibrated() const;
	virtual ProbeDefinition getProbeSector() const;
	virtual double getTimestamp() const;

	virtual double getTooltipOffset() const;
	virtual void setTooltipOffset(double val);

	virtual Transform3D getCalibration_sMt() const;
	virtual std::map<int, Vector3D> getReferencePoints() const;


	virtual TimedTransformMapPtr getPositionHistory() { return mBase->getPositionHistory(); }
	virtual bool isInitialized() const	{ return mBase->isInitialized(); }
	virtual ProbePtr getProbe() const { return mBase->getProbe(); }
	virtual bool hasReferencePointWithId(int id) { return mBase->hasReferencePointWithId(id); }
	virtual TimedTransformMap getSessionHistory(double startTime, double stopTime) { return mBase->getSessionHistory(startTime, stopTime); }

	// extensions
	ToolPtr getBase() { return mBase; }

private slots:
	void timeChangedSlot();
private:
	ToolPtr mBase;
	PlaybackTimePtr mTime;

	bool mVisible;
	double mTimestamp;
	Transform3D m_rMpr;
};

} /* namespace cx */
#endif /* CXPLAYBACKTOOL_H_ */
