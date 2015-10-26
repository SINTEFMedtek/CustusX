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

#ifndef CXACQUISITIONSERVICEPROXY_H
#define CXACQUISITIONSERVICEPROXY_H

#include "cxAcquisitionService.h"
#include "cxServiceTrackerListener.h"
namespace cx
{

/** \brief Always provides an AcqusitionService
 *
 * Use the Proxy design pattern.
 * Uses ServiceTrackerListener to either provide an
 * implementation of AcqusitionService or
 * the null object (AcqusitionServiceNull)
 *
 *  \ingroup org_custusx_acqiusition
 *  \date 2014-11-26
 *  \author Ole Vegard Solberg, SINTEF
 */
class org_custusx_acquisition_EXPORT AcquisitionServiceProxy : public AcquisitionService
{
	Q_OBJECT
public:
//	static AcquisitionServicePtr create(ctkPluginContext *pluginContext);
	AcquisitionServiceProxy(ctkPluginContext *context);
	~AcquisitionServiceProxy() {}

	virtual bool isNull();

	virtual RecordSessionPtr getLatestSession();
	virtual std::vector<RecordSessionPtr> getSessions();

	virtual bool isReady(TYPES context) const;
	virtual QString getInfoText(TYPES context) const;
	virtual STATE getState() const;
	virtual void startRecord(TYPES context, QString category, RecordSessionPtr session);
	virtual void stopRecord();
	virtual void cancelRecord();
	virtual void startPostProcessing();
	virtual void stopPostProcessing();

	virtual int getNumberOfSavingThreads() const;

private:
	ctkPluginContext *mPluginContext;
	AcquisitionServicePtr mAcquisitionService;
	boost::shared_ptr<ServiceTrackerListener<AcquisitionService> > mServiceListener;

	void initServiceListener();
	void onServiceAdded(AcquisitionService *service);
	void onServiceRemoved(AcquisitionService *service);
};

} //cx
#endif // CXACQUISITIONSERVICEPROXY_H
