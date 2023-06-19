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
#include "cxRegistrationService.h"
#include "cxRegistrationTransform.h"
#include "cxPatientModelService.h"
#include "cxViewService.h"
#include "cxVolumeHelpers.h"
#include "cxLogger.h"
#include "cxFilePathProperty.h"
#include "cxImage.h"
#include "cxFileManagerService.h"

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

	if(!mRunningTransformix)
	{
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

		doRegistration(delta_pre_rMd, desc);
	}
	// add nonlinear data AFTER registering - we dont want these data to be double-registered!
	this->addNonlinearData();
	this->deformAdditionalImage();
}

void ElastixManager::doRegistration(Transform3D delta_pre_rMd, QString desc)
{
	ImagePtr deformImage = mServices->patient()->getData<Image>(mParameters->getDeformImage());
	QString parentSpace;
	if(deformImage)
		parentSpace = removeParent(deformImage);

	mServices->registration()->addImage2ImageRegistration(delta_pre_rMd, desc);//Postpone - but avoid double registration through parent system

	if(deformImage)
	{
		setParent(deformImage, parentSpace);
		mDelta_pre_rMd = delta_pre_rMd;
		mDesc = desc;
		QTimer::singleShot(0, this, SLOT(postponedRegistration()));
	}
}

QString ElastixManager::removeParent(ImagePtr image)
{
	if(!image)
		return QString();

	QString parentSpace = image->getParentSpace();
	RegistrationHistoryPtr regHistory = image->get_rMd_History();
	regHistory->setParentSpace(QString());
	return parentSpace;
}

void ElastixManager::setParent(ImagePtr image, QString parentSpace)
{
	if(!image)
		return;
	RegistrationHistoryPtr regHistory = image->get_rMd_History();
	regHistory->setParentSpace(parentSpace);
}

//Move deformImage last, after transformix is finihed
void ElastixManager::postponedRegistration()
{
	if(mExecuter->isRunning())
	{
		QTimer::singleShot(200, this, SLOT(postponedRegistration()));
		return;
	}

	ImagePtr deformImage = mServices->patient()->getData<Image>(mParameters->getDeformImage());
	if(!deformImage || !mLastNonLonearImage)
		return;

	DataPtr movingData = mServices->registration()->getMovingData();

	//Need to temporarily remove parents from deformImage, and the nonlinear copy of deformImage to prevent parents and children to be moved
	QString parentSpace = removeParent(deformImage);
	QString parentSpaceNl = removeParent(mLastNonLonearImage);

	mServices->registration()->setMovingData(deformImage);
	mServices->registration()->addImage2ImageRegistration(mDelta_pre_rMd, mDesc);
	mServices->registration()->setMovingData(movingData);//Set the original movingData back

	//Add parents
	setParent(deformImage, parentSpace);
	setParent(mLastNonLonearImage, parentSpaceNl);
}

/**If the registration is nonlinear, add the (last) volume
 * to CustusX.
 *
 */
void ElastixManager::addNonlinearData()
{
	bool ok = true;
	QString nonlinearVolumeFilename = mExecuter->getNonlinearResultVolume(&ok, mRunningTransformix);

	if (!ok)
		return;

	ImagePtr baseImage = getNonLinearBaseImage();
	ImagePtr raw = boost::dynamic_pointer_cast<Image>(mServices->file()->load(nonlinearVolumeFilename, nonlinearVolumeFilename));

	if (!raw)
	{
		report(QString("Failed to import nonlinear volume %1").arg(nonlinearVolumeFilename));
		return;
	}

	QString uid = baseImage->getUid() + "_nl%1";
	QString name = baseImage->getName()+" nl%1";

	ImagePtr nlVolume;
	nlVolume = createDerivedImage(mServices->patient(),
								  uid, name,
								  raw->getBaseVtkImageData(), baseImage);


	// volume is resampled into the space of the fixed data:
	nlVolume->get_rMd_History()->setRegistration(mServices->registration()->getFixedData()->get_rMd());

	mServices->patient()->insertData(nlVolume);
	mLastNonLonearImage = nlVolume;

	report(QString("Added volume %1, created by a nonlinear transform").arg(nlVolume->getName()));
}

ImagePtr ElastixManager::getNonLinearBaseImage()
{
	if(mRunningTransformix)
	{
		ImagePtr deformImage = mServices->patient()->getData<Image>(mParameters->getDeformImage());
		return deformImage;
	}
	ImagePtr movingImage = boost::dynamic_pointer_cast<Image>(mServices->registration()->getMovingData());
	return movingImage;
}

void ElastixManager::deformAdditionalImage()
{
	//Only run Transformix once
	if(mRunningTransformix)
	{
		mRunningTransformix = false;
		return;
	}
	mRunningTransformix = true;
	ImagePtr deformImage = getNonLinearBaseImage();
	if(!deformImage)
		return;

	mExecuter->runTransformix(deformImage);

}


} /* namespace cx */
