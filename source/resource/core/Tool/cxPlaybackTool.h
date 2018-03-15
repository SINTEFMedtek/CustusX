/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXPLAYBACKTOOL_H_
#define CXPLAYBACKTOOL_H_

#include "cxToolImpl.h"

#include "cxResourceExport.h"

namespace cx
{

typedef boost::shared_ptr<class PlaybackTool> PlaybackToolPtr;
typedef boost::shared_ptr<class PlaybackTime> PlaybackTimePtr;


/** \brief A tool used during playback
 *
 * \date Mar 29, 2012
 * \author Christian Askeland, SINTEF
 *
 * \ingroup org_custusx_core_tracking
 */
class cxResource_EXPORT PlaybackTool: public ToolImpl
{
Q_OBJECT
public:
	explicit PlaybackTool(ToolPtr base, PlaybackTimePtr time);
	virtual ~PlaybackTool();
	virtual std::set<Type> getTypes() const;
	virtual vtkPolyDataPtr getGraphicsPolyData() const;
	virtual void setTransformSaveFile(const QString& filename) {}
	virtual Transform3D get_prMt() const;
	virtual bool getVisible() const;
	virtual QString getUid() const;
	virtual QString getName() const;
	virtual bool isCalibrated() const;
	virtual double getTimestamp() const;

	virtual double getTooltipOffset() const;
	virtual void setTooltipOffset(double val);

	virtual Transform3D getCalibration_sMt() const;
	virtual std::map<int, Vector3D> getReferencePoints() const;


	virtual TimedTransformMapPtr getPositionHistory() { return mBase->getPositionHistory(); }
	virtual bool isInitialized() const;
	virtual ProbePtr getProbe() const { return mBase->getProbe(); }
	virtual bool hasReferencePointWithId(int id) { return mBase->hasReferencePointWithId(id); }
	virtual TimedTransformMap getSessionHistory(double startTime, double stopTime) { return mBase->getSessionHistory(startTime, stopTime); }

	virtual void set_prMt(const Transform3D& prMt, double timestamp);
	virtual void setVisible(bool vis);

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
