/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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
