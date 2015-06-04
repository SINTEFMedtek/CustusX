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


#include "cxElastixManager.h"

#include <QDir>

#include "cxTime.h"
#include "cxProfile.h"
#include "cxElastixExecuter.h"
#include "cxSettings.h"
#include "cxDataReaderWriter.h"
#include "cxRegistrationService.h"
#include "cxRegistrationTransform.h"
#include "cxPatientModelService.h"
#include "cxViewService.h"
#include "cxVolumeHelpers.h"
#include "cxLogger.h"
#include "cxFilePathProperty.h"

namespace cx
{

ElastixManager::ElastixManager(RegServices services) :
	mServices(services)
{
	mOptions = profile()->getXmlSettings().descend(ElastixParameters::getConfigUid());

	mParameters.reset(new ElastixParameters(mOptions));
	connect(mParameters.get(), SIGNAL(elastixParametersChanged()), this, SIGNAL(elastixChanged()));

	mDisplayProcessMessages = BoolProperty::initialize("displayProcessMessages",
		"Show Messages",
		"Display messages from the running registration process in CustusX",
		false,
		mOptions.getElement());

	mDisableRendering = BoolProperty::initialize("disableRendering",
		"Disable Rendering",
		"Disable rendering while running process.\n"
		"Can be used to avoid clash in GPU usage.",
		false,
		mOptions.getElement());

	mExecuter.reset(new ElastixExecuter(services));
	connect(mExecuter.get(), SIGNAL(finished()), this, SLOT(executionFinishedSlot()));
	connect(mExecuter.get(), SIGNAL(aboutToStart()), this, SLOT(preprocessExecuter()));
}

ElastixManager::~ElastixManager()
{
}

void ElastixManager::execute()
{
	mExecuter->execute();
}

void ElastixManager::preprocessExecuter()
{
	QStringList parameterFiles = mParameters->getActiveParameterFiles();
	QString timestamp = QDateTime::currentDateTime().toString(timestampSecondsFormat());
	QDir outDir(mServices.patientModelService->getActivePatientFolder()+"/"+mParameters->getConfigUid()+"/"+timestamp);

	mExecuter->setDisplayProcessMessages(mDisplayProcessMessages->getValue());
	mExecuter->setInput(mParameters->getActiveExecutable()->getEmbeddedPath().getAbsoluteFilepath(),
					 mServices.registrationService->getFixedData(),
					 mServices.registrationService->getMovingData(),
	         outDir.absolutePath(),
	         parameterFiles);

	if (mDisableRendering->getValue())
	{
		mServices.visualizationService->enableRender(false);
	}
}

void ElastixManager::executionFinishedSlot()
{
	if (mDisableRendering->getValue())
		mServices.visualizationService->enableRender(true);

	bool ok = false;
	Transform3D mMf = mExecuter->getAffineResult_mMf(&ok);

	if (!ok)
		return;

//	std::cout << "ElastixManager::executionFinishedSlot(), Linear Result mMf: \n" << mMf << std::endl;

	QStringList parameterFiles = mParameters->getActiveParameterFiles();
	QString desc = QString("Image2Image [exe=%1]").arg(QFileInfo(mParameters->getActiveExecutable()->getValue()).fileName());
	for (unsigned i=0; i<parameterFiles.size(); ++i)
		desc += QString("[par=%1]").arg(QFileInfo(parameterFiles[i]).fileName());

	// Start with fMr * D * rMm = fMm'
	// where the lhs is the existing data and the delta that is input to regmanager,
	// and the rhs is the (inverse of the) output from ElastiX.
	// This gives
	// D = rMf * fMm' * mMr
	// as the input to regmanager applyImage2ImageRegistration()

	Transform3D delta_pre_rMd =
		mServices.registrationService->getFixedData()->get_rMd()
		* mMf.inv()
		* mServices.registrationService->getMovingData()->get_rMd().inv();

//	std::cout << "ElastixManager::executionFinishedSlot(), delta_pre_rMd: \n" << delta_pre_rMd << std::endl;
//	std::cout << "ElastixManager::executionFinishedSlot(), expected new rMdm: \n" << mServices.registrationService->getFixedData()->get_rMd() * mMf.inv() << std::endl;

//	mServices.registrationService->applyImage2ImageRegistration(mMf.inv(), desc);
	mServices.registrationService->applyImage2ImageRegistration(delta_pre_rMd, desc);

	// add nonlinear data AFTER registering - we dont want these data to be double-registered!
	this->addNonlinearData();
}

/**If the registration is nonlinear, add the (last) volume
 * to CustusX.
 *
 */
void ElastixManager::addNonlinearData()
{
	bool ok = true;
	QString nonlinearVolumeFilename = mExecuter->getNonlinearResultVolume(&ok);

	if (!ok)
		return;

	ImagePtr movingImage = boost::dynamic_pointer_cast<Image>(mServices.registrationService->getMovingData());
	ImagePtr raw = boost::dynamic_pointer_cast<Image>(MetaImageReader().load(nonlinearVolumeFilename, nonlinearVolumeFilename));

	if (!raw)
	{
		report(QString("Failed to import nonlinear volume %1").arg(nonlinearVolumeFilename));
		return;
	}

	QString uid = movingImage->getUid() + "_nl%1";
	QString name = movingImage->getName()+" nl%1";

	ImagePtr nlVolume = createDerivedImage(mServices.patientModelService,
										 uid, name,
										 raw->getBaseVtkImageData(), movingImage);

	// volume is resampled into the space of the fixed data:
	nlVolume->get_rMd_History()->setRegistration(mServices.registrationService->getFixedData()->get_rMd());

	mServices.patientModelService->insertData(nlVolume);

	report(QString("Added volume %1, created by a nonlinear transform").arg(nlVolume->getName()));
}

} /* namespace cx */
