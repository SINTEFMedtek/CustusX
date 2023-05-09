/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef CXELASTIXMANAGER_H_
#define CXELASTIXMANAGER_H_

#include <QObject>
#include "cxForwardDeclarations.h"
#include "cxXmlOptionItem.h"
#include "cxBoolProperty.h"
#include "cxStringProperty.h"
#include "cxElastixParameters.h"
#include "cxRegServices.h"
#include "cxTransform3D.h"

namespace cx
{
/**
 * \file
 * \addtogroup org_custusx_registration_method_commandline
 * @{
 */

typedef boost::shared_ptr<class ElastixExecuter> ElastixExecuterPtr;

/**
 * \brief Manager for interfacing to the ElastiX registration package.
 *
 * \date Feb 4, 2012
 * \author Christian Askeland, SINTEF
 */
class ElastixManager : public QObject
{
	Q_OBJECT
public:
	ElastixManager(RegServicesPtr services);
	virtual ~ElastixManager();

	BoolPropertyPtr getDisplayProcessMessages() { return mDisplayProcessMessages; }
	BoolPropertyPtr getDisableRendering() { return mDisableRendering; }
	ElastixExecuterPtr getExecuter() { return mExecuter; }
	ElastixParametersPtr getParameters() { return mParameters; }

	void execute();
signals:
	void elastixChanged();

private slots:
	void executionFinishedSlot();
	void preprocessExecuter();
	void postponedRegistration();

private:
	void addNonlinearData();
	void deformAdditionalImage();
	ImagePtr getNonLinearBaseImage();
	QString removeParent(ImagePtr image);
	void setParent(ImagePtr image, QString parentSpace);
	void doRegistration(Transform3D delta_pre_rMd, QString desc);

	ElastixParametersPtr mParameters;
	XmlOptionFile mOptions;
	BoolPropertyPtr mDisplayProcessMessages;
	BoolPropertyPtr mDisableRendering;
	ElastixExecuterPtr mExecuter;
	RegServicesPtr mServices;
	bool mRunningTransformix = false;

	Transform3D mDelta_pre_rMd;
	QString mDesc;
	ImagePtr mLastNonLonearImage;
};
typedef boost::shared_ptr<ElastixManager> ElastixManagerPtr;

/**
 * @}
 */
} /* namespace cx */
#endif /* CXELASTIXMANAGER_H_ */
