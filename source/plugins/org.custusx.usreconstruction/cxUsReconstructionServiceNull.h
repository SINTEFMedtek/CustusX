/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXUSRECONSTRUCTIONSERVICENULL_H
#define CXUSRECONSTRUCTIONSERVICENULL_H

#include "org_custusx_usreconstruction_Export.h"
#include "cxUsReconstructionService.h"

namespace cx
{

/**
 * \ingroup org_custusx_usreconstruction
 */
class org_custusx_usreconstruction_EXPORT UsReconstructionServiceNull : public UsReconstructionService
{
Q_OBJECT

public:
	UsReconstructionServiceNull();

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
	virtual void setOutputRelativePath(QString path);
	virtual void setOutputBasePath(QString path);
	virtual void startReconstruction();
	virtual std::set<cx::TimedAlgorithmPtr> getThreadedReconstruction();
	virtual ReconstructionMethodService* createAlgorithm();

	virtual ReconstructCore::InputParams createCoreParameters();

	virtual bool isNull();

public slots:
	virtual void newDataOnDisk(QString mhdFilename);

private:
	void printWarning() const;

};

} //cx

#endif // CXUSRECONSTRUCTIONSERVICENULL_H
