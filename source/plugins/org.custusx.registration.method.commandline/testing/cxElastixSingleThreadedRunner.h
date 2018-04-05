/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXELASTIXSINGLETHREADEDRUNNER_H_
#define CXELASTIXSINGLETHREADEDRUNNER_H_

#include "cxtest_org_custusx_registration_method_commandline_export.h"

#include <QObject>
#include "cxTransform3D.h"
#include "cxForwardDeclarations.h"
#include "cxRegServices.h"

namespace cx
{

typedef boost::shared_ptr<class ElastixExecuter> ElastixExecuterPtr;
typedef boost::shared_ptr<class ElastixParameters> ElastixParametersPtr;


/** Helper class for running ElastixExecuter in a test.
 *
 *  \date Oct 8, 2012
 *  \author christiana
 */
class CXTEST_ORG_CUSTUSX_REGISTRATION_METHOD_COMMANDLINE_EXPORT ElastixSingleThreadedRunner : public QObject
{
	Q_OBJECT
public:
	bool registerLinear(
        DataPtr fixed,
        DataPtr moving,
		ElastixParametersPtr preset,
        Transform3D* result);

	ElastixSingleThreadedRunner(RegServicesPtr services);
	virtual ~ElastixSingleThreadedRunner();

private slots:
	void preprocessExecuter();
	void executionFinishedSlot();
private:
	ElastixExecuterPtr mExecuter;
	bool mCompleted;
	bool mFailed;
	Transform3D m_mMf;

};

}

#endif /* CXELASTIXSINGLETHREADEDRUNNER_H_ */
