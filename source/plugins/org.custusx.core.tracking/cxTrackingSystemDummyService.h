/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef CXTRACKINGSYSTEMDUMMYSERVICE_H
#define CXTRACKINGSYSTEMDUMMYSERVICE_H

#include "org_custusx_core_tracking_Export.h"
#include "cxTrackingSystemService.h"

namespace cx
{

/**
 * \brief Interface towards a dummy tracking system.
 * \ingroup org_custusx_core_tracking
 *
 * \date 2014-11-01
 * \author Christian Askeland, SINTEF
 */
class org_custusx_core_tracking_EXPORT TrackingSystemDummyService : public TrackingSystemService
{
Q_OBJECT

public:
	TrackingSystemDummyService(DummyToolPtr tool);
	virtual ~TrackingSystemDummyService();

	virtual QString getUid() const { return "org.custusx.core.tracking.system.dummy"; }
	virtual std::vector<ToolPtr> getTools();
	virtual ToolPtr getReference() { return ToolPtr(); }

	virtual Tool::State getState() const;
	virtual void setState(const Tool::State val);

	virtual void setLoggingFolder(QString loggingFolder); ///<\param loggingFolder path to the folder where logs should be saved
	virtual TrackerConfigurationPtr getConfiguration();

private:
	std::vector<DummyToolPtr> mTools; ///< all tools
	Tool::State mState;
};


} // namespace cx


#endif // CXTRACKINGSYSTEMDUMMYSERVICE_H
