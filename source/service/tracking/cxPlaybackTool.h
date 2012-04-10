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

#include "sscTool.h"
#include "sscToolManager.h"
#include "cxPlaybackTime.h"

namespace cx
{

typedef boost::shared_ptr<class PlaybackTool> PlaybackToolPtr;

/**\brief A tool used during playback
 *
 * \date Mar 29, 2012
 * \author Christian Askeland, SINTEF
 *
 * \ingroup sscTool
 */
class PlaybackTool: public ssc::Tool
{
Q_OBJECT
public:
	explicit PlaybackTool(ssc::ToolPtr base, PlaybackTimePtr time);
	virtual ~PlaybackTool();
#ifdef SSC_USE_DEPRECATED_TOOL_ENUM
	virtual Type getType() const;
#endif
	virtual QString getGraphicsFileName() const;
	virtual vtkPolyDataPtr getGraphicsPolyData() const;
	virtual void saveTransformsAndTimestamps() {}
	virtual void setTransformSaveFile(const QString& filename) {}
	virtual ssc::Transform3D get_prMt() const;
	virtual bool getVisible() const;
	virtual QString getUid() const;
	virtual QString getName() const;
	virtual bool isCalibrated() const;
	virtual ssc::ProbeData getProbeSector() const;
	virtual double getTimestamp() const;

	virtual double getTooltipOffset() const;
	virtual void setTooltipOffset(double val);

	virtual ssc::Transform3D getCalibration_sMt() const;
	virtual std::map<int, ssc::Vector3D> getReferencePoints() const;

	ssc::ToolPtr getBase() { return mBase; }

#ifdef SSC_USE_DEPRECATED_TOOL_ENUM
#else
	virtual bool isManual() const { return mBase->isManual(); }
	virtual bool isReference() const { return mBase->isReference(); }
	virtual bool isPointer() const { return mBase->isPointer(); }
	virtual bool isProbe() const { return mBase->isProbe(); }
	virtual bool isMicroscope() const { return mBase->isMicroscope(); }
#endif
	virtual ssc::TimedTransformMapPtr getPositionHistory() { return mBase->getPositionHistory(); }
	virtual bool isInitialized() const	{ return mBase->isInitialized(); }
	virtual ssc::ProbePtr getProbe() const { return mBase->getProbe(); }
	virtual bool hasReferencePointWithId(int id) { return mBase->hasReferencePointWithId(id); }
	virtual ssc::TimedTransformMap getSessionHistory(double startTime, double stopTime) { return mBase->getSessionHistory(startTime, stopTime); }


private slots:
	void timeChangedSlot();
private:
	ssc::ToolPtr mBase;
	PlaybackTimePtr mTime;

	bool mVisible;
	double mTimestamp;
	ssc::Transform3D m_rMpr;
};

} /* namespace cx */
#endif /* CXPLAYBACKTOOL_H_ */
