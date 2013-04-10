// This file is part of CustusX, an Image Guided Therapy Application.
//
// Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
//
// CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
// code and binaries can only be used by SMT and those with explicit permission
// from SMT. CustusX shall not be distributed to anyone else.
//
// CustusX is a research tool. It is NOT intended for use or certified for use
// in a normal clinical setting. SMT does not take responsibility for its use
// in any way.
//
// See CustusX_License.txt for more information.

#ifndef CXELASTIXSINGLETHREADEDRUNNER_H_
#define CXELASTIXSINGLETHREADEDRUNNER_H_

#include <QObject>
#include "sscTransform3D.h"
#include "cxForwardDeclarations.h"

namespace cx
{

typedef boost::shared_ptr<class ElastixExecuter> ElastixExecuterPtr;
typedef boost::shared_ptr<class ElastixParameters> ElastixParametersPtr;


/** Helper class for running ElastixExecuter in a test.
 *
 *  \date Oct 8, 2012
 *  \author christiana
 */
class ElastixSingleThreadedRunner : public QObject
{
	Q_OBJECT
public:
	bool registerLinear(
        ssc::DataPtr fixed,
        ssc::DataPtr moving,
		ElastixParametersPtr preset,
        ssc::Transform3D* result);

	ElastixSingleThreadedRunner();
	virtual ~ElastixSingleThreadedRunner();

private slots:
	void preprocessExecuter();
	void executionFinishedSlot();
private:
	ElastixExecuterPtr mExecuter;
	bool mCompleted;
	ssc::Transform3D m_mMf;

};

}

#endif /* CXELASTIXSINGLETHREADEDRUNNER_H_ */
