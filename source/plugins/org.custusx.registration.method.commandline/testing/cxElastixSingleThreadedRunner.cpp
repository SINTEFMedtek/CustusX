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
