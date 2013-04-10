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


#include "cxElastixManager.h"

#include <QDir>
#include "sscLogger.h"
#include "sscTime.h"
#include "cxDataLocations.h"
#include "cxElastixExecuter.h"
#include "cxPatientService.h"
#include "cxPatientData.h"
#include "cxSettings.h"
#include "sscDataManagerImpl.h"

namespace cx
{

ElastixManager::ElastixManager(RegistrationManagerPtr regManager) : mRegistrationManager(regManager)
{
	mOptions = ssc::XmlOptionFile(DataLocations::getXmlSettingsFile(), "CustusX").descend("elastix");

	mParameters.reset(new ElastixParameters(mOptions));
	connect(mParameters.get(), SIGNAL(elastixParametersChanged()), this, SIGNAL(elastixChanged()));

	mDisplayProcessMessages = ssc::BoolDataAdapterXml::initialize("displayProcessMessages",
		"Show Messages",
		"Display messages from the running registration process in CustusX",
		false,
		mOptions.getElement());

	mExecuter.reset(new ElastixExecuter());
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
	// Not necessary - executer makes sure that file transform is read and deviations
	// accounted for in t0.
	//
	// We MUST save the patient before registering.
	// elastiX uses the Offset+TransformMatrix values, so they must be up to date.
	//patientService()->getPatientData()->savePatient();

	QStringList parameterFiles = mParameters->getActiveParameterFiles();
	QString timestamp = QDateTime::currentDateTime().toString(ssc::timestampSecondsFormat());
	QDir outDir(patientService()->getPatientData()->getActivePatientFolder()+"/elastix/"+timestamp);

	mExecuter->setDisplayProcessMessages(mDisplayProcessMessages->getValue());
	mExecuter->setInput(mParameters->getActiveExecutable(),
	         mRegistrationManager->getFixedData(),
	         mRegistrationManager->getMovingData(),
	         outDir.absolutePath(),
	         parameterFiles);
}

void ElastixManager::executionFinishedSlot()
{
	bool ok = false;
	ssc::Transform3D mMf = mExecuter->getAffineResult_mMf(&ok);

	if (!ok)
		return;

	std::cout << "ElastixManager::executionFinishedSlot(), Linear Result mMf: \n" << mMf << std::endl;

	QStringList parameterFiles = mParameters->getActiveParameterFiles();
	QString desc = QString("Image2Image [exe=%1]").arg(QFileInfo(mParameters->getActiveExecutable()).fileName());
	for (unsigned i=0; i<parameterFiles.size(); ++i)
		desc += QString("[par=%1]").arg(QFileInfo(parameterFiles[i]).fileName());

	// Start with fMr * D * rMm = fMm'
	// where the lhs is the existing data and the delta that is input to regmanager,
	// and the rhs is the (inverse of the) output from ElastiX.
	// This gives
	// D = rMf * fMm' * mMr
	// as the input to regmanager applyImage2ImageRegistration()

	ssc::Transform3D delta_pre_rMd =
		mRegistrationManager->getFixedData()->get_rMd()
		* mMf.inv()
		* mRegistrationManager->getMovingData()->get_rMd().inv();

	std::cout << "ElastixManager::executionFinishedSlot(), delta_pre_rMd: \n" << delta_pre_rMd << std::endl;
	std::cout << "ElastixManager::executionFinishedSlot(), expected new rMdm: \n" << mRegistrationManager->getFixedData()->get_rMd() * mMf.inv() << std::endl;

//	mRegistrationManager->applyImage2ImageRegistration(mMf.inv(), desc);
	mRegistrationManager->applyImage2ImageRegistration(delta_pre_rMd, desc);

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

	ssc::ImagePtr movingImage = boost::dynamic_pointer_cast<ssc::Image>(mRegistrationManager->getMovingData());
	ssc::ImagePtr raw = boost::dynamic_pointer_cast<ssc::Image>(ssc::MetaImageReader().load(nonlinearVolumeFilename, nonlinearVolumeFilename));

	QString uid = movingImage->getUid() + "_nl%1";
	QString name = movingImage->getName()+" nl%1";
	ssc::ImagePtr nlVolume = ssc::dataManager()->createDerivedImage(raw->getBaseVtkImageData(), uid, name, movingImage);

	if (!nlVolume)
	{
		ssc::messageManager()->sendInfo(QString("Failed to import nonlinear volume %1").arg(nonlinearVolumeFilename));
		return;
	}

	// volume is resampled into the space of the fixed data:
	nlVolume->get_rMd_History()->setRegistration(mRegistrationManager->getFixedData()->get_rMd());

	ssc::dataManager()->loadData(nlVolume);
	ssc::dataManager()->saveImage(nlVolume, patientService()->getPatientData()->getActivePatientFolder());

	ssc::messageManager()->sendInfo(QString("Added volume %1, created by a nonlinear transform").arg(nlVolume->getName()));
}

} /* namespace cx */
