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
#ifndef CXTRACKINGSYSTEMDUMMYSERVICE_H
#define CXTRACKINGSYSTEMDUMMYSERVICE_H

#include "cxTrackingServiceExport.h"
#include "cxTrackingSystemService.h"

namespace cx
{

/**
 * \brief Interface towards a dummy tracking system.
 * \ingroup cx_service_tracking
 *
 * \date 2014-11-01
 * \author Christian Askeland, SINTEF
 */
class cxTrackingService_EXPORT TrackingSystemDummyService : public TrackingSystemService
{
Q_OBJECT

public:
	TrackingSystemDummyService(DummyToolPtr tool);
	virtual ~TrackingSystemDummyService();

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
