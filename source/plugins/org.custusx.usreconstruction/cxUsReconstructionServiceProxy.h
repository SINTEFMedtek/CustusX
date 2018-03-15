/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXUSRECONSTRUCTIONSERVICEPROXY_H
#define CXUSRECONSTRUCTIONSERVICEPROXY_H

#include "org_custusx_usreconstruction_Export.h"

#include "cxUsReconstructionService.h"
#include "cxServiceTrackerListener.h"
class ctkPluginContext;

namespace cx
{

/**
 * \ingroup org_custusx_usreconstruction
 */
class org_custusx_usreconstruction_EXPORT UsReconstructionServiceProxy : public UsReconstructionService
{

	Q_OBJECT

public:
	UsReconstructionServiceProxy(ctkPluginContext *pluginContext);

	virtual void selectData(QString filename, QString calFilesPath = "");
	virtual void selectData(USReconstructInputData data);

	virtual QString getSelectedFilename() const;
	virtual USReconstructInputData getSelectedFileData();
//	virtual ReconstructParamsPtr getParams();
	virtual PropertyPtr getParam(QString uid);
	virtual std::vector<PropertyPtr> getAlgoOptions();
	virtual XmlOptionFile getSettings();
	virtual OutputVolumeParams getOutputVolumeParams() const;

	virtual void setOutputVolumeParams(const OutputVolumeParams& par);
//	virtual void setOutputRelativePath(QString path);
//	virtual void setOutputBasePath(QString path);
	virtual void startReconstruction();
	virtual std::set<cx::TimedAlgorithmPtr> getThreadedReconstruction();
	virtual ReconstructionMethodService* createAlgorithm();

	virtual ReconstructCore::InputParams createCoreParameters();

	virtual bool isNull();

public slots:
	virtual void newDataOnDisk(QString mhdFilename);
private:
	void initServiceListener();
	void onServiceAdded(UsReconstructionService* service);
	void onServiceRemoved(UsReconstructionService *service);

	ctkPluginContext *mPluginContext;
	UsReconstructionServicePtr mUsReconstructionService;
	boost::shared_ptr<ServiceTrackerListener<UsReconstructionService> > mServiceListener;
};

} //cx

#endif // CXUSRECONSTRUCTIONSERVICEPROXY_H
