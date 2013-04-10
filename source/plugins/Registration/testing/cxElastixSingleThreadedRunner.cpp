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

#include "cxElastixSingleThreadedRunner.h"
#include "cxElastixExecuter.h"
#include "sscXmlOptionItem.h"
#include <QDir>
#include "cxDataLocations.h"
#include <qapplication.h>
#include "sscTime.h"
#include "sscTypeConversions.h"
#include "sscLogger.h"
#include "cxElastixParameters.h"

namespace cx
{

ElastixSingleThreadedRunner::ElastixSingleThreadedRunner()
{
	mExecuter.reset(new ElastixExecuter());
	connect(mExecuter.get(), SIGNAL(finished()), this, SLOT(executionFinishedSlot()));
	connect(mExecuter.get(), SIGNAL(aboutToStart()), this, SLOT(preprocessExecuter()));
	mCompleted = false;
}

ElastixSingleThreadedRunner::~ElastixSingleThreadedRunner()
{
}

bool ElastixSingleThreadedRunner::registerLinear(
    ssc::DataPtr fixed,
    ssc::DataPtr moving,
	ElastixParametersPtr preset,
    ssc::Transform3D* result)
{
	mCompleted = false;
	QString outPath = cx::DataLocations::getTestDataPath() + "/temp/elastix/" + QDateTime::currentDateTime().toString(ssc::timestampMilliSecondsFormat() + "/");

	mExecuter->setDisplayProcessMessages(false);
	mExecuter->setDisplayProcessMessages(true);
	mExecuter->setInput(preset->getActiveExecutable(),
	         fixed,
	         moving,
	         outPath,
			 preset->getActiveParameterFiles());

	while (!mCompleted)
		qApp->processEvents();

	*result = m_mMf;
	return true;
}

void ElastixSingleThreadedRunner::preprocessExecuter()
{
}

void ElastixSingleThreadedRunner::executionFinishedSlot()
{
	bool ok = false;
	m_mMf = mExecuter->getAffineResult_mMf(&ok);
	mCompleted = true;

	if (!ok)
		return;

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

//	ssc::Transform3D delta_pre_rMd =
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
