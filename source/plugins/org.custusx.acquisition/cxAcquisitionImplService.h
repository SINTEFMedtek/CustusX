/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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
