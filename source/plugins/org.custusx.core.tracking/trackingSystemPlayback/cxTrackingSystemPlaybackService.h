/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors
   may be used to endorse or promote products derived from this software
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
