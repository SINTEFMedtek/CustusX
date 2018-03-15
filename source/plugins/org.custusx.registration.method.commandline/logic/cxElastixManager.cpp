/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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

ElastixManager::ElastixManager(RegServicesPtr services) :
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
	QDir outDir(mServices->patient()->getActivePatientFolder()+"/"+mParameters->getConfigUid()+"/"+timestamp);

	mExecuter->setDisplayProcessMessages(mDisplayProcessMessages->getValue());
	mExecuter->setInput(mParameters->getActiveExecutable()->getEmbeddedPath().getAbsoluteFilepath(),
					 mServices->registration()->getFixedData(),
					 mServices->registration()->getMovingData(),
	         outDir.absolutePath(),
	         parameterFiles);

	if (mDisableRendering->getValue())
	{
		mServices->view()->enableRender(false);
	}
}

void ElastixManager::executionFinishedSlot()
{
	if (mDisableRendering->getValue())
		mServices->view()->enableRender(true);

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
		mServices->registration()->getFixedData()->get_rMd()
		* mMf.inv()
		* mServices->registration()->getMovingData()->get_rMd().inv();

//	std::cout << "ElastixManager::executionFinishedSlot(), delta_pre_rMd: \n" << delta_pre_rMd << std::endl;
//	std::cout << "ElastixManager::executionFinishedSlot(), expected new rMdm: \n" << mServices->registration()->getFixedData()->get_rMd() * mMf.inv() << std::endl;

//	mServices->registration()->addImage2ImageRegistration(mMf.inv(), desc);
	mServices->registration()->addImage2ImageRegistration(delta_pre_rMd, desc);

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

	ImagePtr movingImage = boost::dynamic_pointer_cast<Image>(mServices->registration()->getMovingData());
	ImagePtr raw = boost::dynamic_pointer_cast<Image>(MetaImageReader().load(nonlinearVolumeFilename, nonlinearVolumeFilename));

	if (!raw)
	{
		report(QString("Failed to import nonlinear volume %1").arg(nonlinearVolumeFilename));
		return;
	}

	QString uid = movingImage->getUid() + "_nl%1";
	QString name = movingImage->getName()+" nl%1";

	ImagePtr nlVolume = createDerivedImage(mServices->patient(),
										 uid, name,
										 raw->getBaseVtkImageData(), movingImage);

	// volume is resampled into the space of the fixed data:
	nlVolume->get_rMd_History()->setRegistration(mServices->registration()->getFixedData()->get_rMd());

	mServices->patient()->insertData(nlVolume);

	report(QString("Added volume %1, created by a nonlinear transform").arg(nlVolume->getName()));
}

} /* namespace cx */
