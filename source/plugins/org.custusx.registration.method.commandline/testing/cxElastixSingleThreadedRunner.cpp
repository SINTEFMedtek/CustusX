/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxElastixSingleThreadedRunner.h"
#include "cxElastixExecuter.h"
#include "cxXmlOptionItem.h"
#include <QDir>
#include "cxDataLocations.h"
#include <qapplication.h>
#include "cxTime.h"
#include "cxTypeConversions.h"

#include "cxElastixParameters.h"
#include "cxFilePathProperty.h"

namespace cx
{

ElastixSingleThreadedRunner::ElastixSingleThreadedRunner(RegServicesPtr services)
{
	mExecuter.reset(new ElastixExecuter(services));
	connect(mExecuter.get(), SIGNAL(finished()), this, SLOT(executionFinishedSlot()));
	connect(mExecuter.get(), SIGNAL(aboutToStart()), this, SLOT(preprocessExecuter()));
	mCompleted = false;
	mFailed = false;
}

ElastixSingleThreadedRunner::~ElastixSingleThreadedRunner()
{
}

bool ElastixSingleThreadedRunner::registerLinear(
    DataPtr fixed,
    DataPtr moving,
	ElastixParametersPtr preset,
    Transform3D* result)
{
	mCompleted = false;
	QString outPath = cx::DataLocations::getTestDataPath() + "/temp/elastix/" + QDateTime::currentDateTime().toString(timestampMilliSecondsFormat() + "/");

	mExecuter->setDisplayProcessMessages(false);
	mExecuter->setDisplayProcessMessages(true);
	bool ok = mExecuter->setInput(preset->getActiveExecutable()->getEmbeddedPath().getAbsoluteFilepath(),
	         fixed,
	         moving,
	         outPath,
			 preset->getActiveParameterFiles());
	if (!ok)
		return false;

	while (!mCompleted)
		qApp->processEvents();

	*result = m_mMf;

	return !mFailed;
}

void ElastixSingleThreadedRunner::preprocessExecuter()
{
}

void ElastixSingleThreadedRunner::executionFinishedSlot()
{
	bool ok = false;
	m_mMf = mExecuter->getAffineResult_mMf(&ok);
	mCompleted = true;
	mFailed = !ok;

//	if (!ok)
//		return;

//	std::cout << "ElastixSingleThreadedRunner::executionFinishedSlot(), Linear Result mMf: \n" << mMf << std::endl;

//	QString desc = QString("Image2Image [exe=%1][par=%2]")
//		.arg(QFileInfo(this->getActiveExecutable()).fileName())
//		.arg(QFileInfo(this->getActiveParameterFile0()).fileName());

	// Start with fMr * D * rMm = fMm'
	// where the lhs is the existing data and the delta that is input to regmanager,
	// and the rhs is the (inverse of the) output from ElastiX.
	// This gives
	// D = rMf * fMm' * mMr
	// as the input to regmanager applyImage2ImageRegistration()

//	Transform3D delta_pre_rMd =
//		mRegistrationManager->getFixedData()->get_rMd()
//		* mMf.inv()
//		* mRegistrationManager->getMovingData()->get_rMd().inv();
//
//	std::cout << "ElastixSingleThreadedRunner::executionFinishedSlot(), delta_pre_rMd: \n" << delta_pre_rMd << std::endl;
//	std::cout << "ElastixSingleThreadedRunner::executionFinishedSlot(), expected new rMdm: \n" << mRegistrationManager->getFixedData()->get_rMd() * mMf.inv() << std::endl;

//	mRegistrationManager->applyImage2ImageRegistration(mMf.inv(), desc);
//	mRegistrationManager->applyImage2ImageRegistration(delta_pre_rMd, desc);

	// add nonlinear data AFTER registering - we dont want these data to be double-registered!
//	this->addNonlinearData();
}



}
