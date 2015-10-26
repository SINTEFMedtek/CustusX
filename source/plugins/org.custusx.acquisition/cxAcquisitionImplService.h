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

#ifndef CXACQUISITIONIMPLSERVICE_H
#define CXACQUISITIONIMPLSERVICE_H

#include "cxAcquisitionService.h"
class ctkPluginContext;
class QDomElement;

namespace cx
{
typedef boost::shared_ptr<class Acquisition> AcquisitionPtr;
typedef boost::shared_ptr<class AcquisitionData> AcquisitionDataPtr;
typedef boost::shared_ptr<class USAcquisition> USAcquisitionPtr;
typedef boost::shared_ptr<class UsReconstructionService> UsReconstructionServicePtr;
typedef boost::shared_ptr<class PatientModelService> PatientModelServicePtr;
typedef boost::shared_ptr<class SessionStorageService> SessionStorageServicePtr;
typedef boost::shared_ptr<class VisServices> VisServicesPtr;

/** \brief Implementation for Acqusition service
 *
 *  \ingroup org_custusx_acqiusition
 *  \date 2014-11-26
 *  \author Ole Vegard Solberg, SINTEF
 */
class org_custusx_acquisition_EXPORT AcquisitionImplService : public AcquisitionService
{
	Q_INTERFACES(cx::AcquisitionService)
public:
	AcquisitionImplService(ctkPluginContext *context);
	virtual ~AcquisitionImplService();
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


private slots:
	void duringClearPatientSlot();
	void duringSavePatientSlot(QDomElement& node);
	void duringLoadPatientSlot(QDomElement& node);
private:
	ctkPluginContext* mContext;
	AcquisitionDataPtr mAcquisitionData;
	AcquisitionPtr mAcquisition;
	UsReconstructionServicePtr mUsReconstructService;
	USAcquisitionPtr mUsAcquisition;
	VisServicesPtr mServices;

	void addXml(QDomNode& dataNode);
	void parseXml(QDomNode& dataNode);
};

typedef boost::shared_ptr<AcquisitionImplService> AcquisitionImplServicePtr;

} //cx

#endif // CXACQUISITIONIMPLSERVICE_H
