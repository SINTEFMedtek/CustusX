/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef CXTRACKINGSYSTEMPLAYBACKSERVICE_H
#define CXTRACKINGSYSTEMPLAYBACKSERVICE_H

#include "org_custusx_core_tracking_Export.h"
#include "cxTrackingSystemService.h"

namespace cx
{
typedef boost::shared_ptr<class PlaybackTime> PlaybackTimePtr;
typedef boost::shared_ptr<class PlaybackTool> PlaybackToolPtr;

/**
 * \brief Interface towards a playback tracking system.
 * \ingroup org_custusx_core_tracking
 *
 * Wraps another tracking system, enabling playback of the tools in that system
 *
 *
 * The original tools are wrapped by playback tools. The original ones are
 * not changed, only their movement is ignored.
 *
 * \date 2014-11-01
 * \author Christian Askeland, SINTEF
 */
class org_custusx_core_tracking_EXPORT TrackingSystemPlaybackService : public TrackingSystemService
{
Q_OBJECT

public:
	TrackingSystemPlaybackService(PlaybackTimePtr controller, std::vector<TrackingSystemServicePtr> base, ManualToolPtr manual);
	virtual ~TrackingSystemPlaybackService();

	virtual QString getUid() const { return "org.custusx.core.tracking.system.playback"; }
	virtual std::vector<ToolPtr> getTools();
	virtual ToolPtr getReference() { return ToolPtr(); }

	virtual Tool::State getState() const;
	virtual void setState(const Tool::State val);

	virtual void setLoggingFolder(QString loggingFolder); ///<\param loggingFolder path to the folder where logs should be saved
	virtual TrackerConfigurationPtr getConfiguration();

	std::vector<TrackingSystemServicePtr> getBase() { return mBases; }

private slots:
	void onToolPositionChanged(Transform3D matrix, double timestamp);

private:
	void start();
	void stop();
	bool forceBaseToConfiguredState();
	void waitForState(TrackingSystemServicePtr base, Tool::State state, int timeout);
	bool isRunning() const;

	std::vector<PlaybackToolPtr> mTools; ///< all tools
	Tool::State mState;
	PlaybackTimePtr mController;
	ManualToolPtr mManual;
	std::vector<TrackingSystemServicePtr> mBases;
};


} // namespace cx


#endif // CXTRACKINGSYSTEMPLAYBACKSERVICE_H
