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

//	bool showStdOut = settings()->value("elastix/showStdOut").toBool();
	mDisplayProcessMessages = ssc::BoolDataAdapterXml::initialize("DisplayElastixProcessMessages",
		"Show Messages",
		"Display messages from the running registration process in CustusX",
		false,
		mOptions.getElement());

	mActiveExecutable = mOptions.getElement().attribute("executable");
	mActiveParameterFile = mOptions.getElement().attribute("parameterFile");
	mExecuter.reset(new ElastixExecuter());
	connect(mExecuter.get(), SIGNAL(finished()), this, SLOT(executionFinishedSlot()));
}

ElastixManager::~ElastixManager()
{
}

void ElastixManager::setActiveParameterFile(QString filename)
{
	mActiveParameterFile = filename;

	mOptions.getElement().setAttribute("parameterFile", mActiveParameterFile);
	emit elastixChanged();
}

QString ElastixManager::getActiveParameterFile() const
{
	return mActiveParameterFile;
}

void ElastixManager::setActiveExecutable(QString filename)
{
	mActiveExecutable = filename;
	mOptions.getElement().setAttribute("executable", mActiveExecutable);
	emit elastixChanged();
}

QString ElastixManager::getActiveExecutable() const
{
	return mActiveExecutable;
}

void ElastixManager::execute()
{
//	SSC_LOG("exec");
//	QDir folder(cx::DataLocations::getRootConfigPath() + "/elastix");
	QString timestamp = QDateTime::currentDateTime().toString(ssc::timestampSecondsFormat());
	QDir outDir(patientService()->getPatientData()->getActivePatientFolder()+"/elastix/"+timestamp);
	mExecuter->setDisplayProcessMessages(mDisplayProcessMessages->getValue());
	mExecuter->run(mActiveExecutable,
	         mRegistrationManager->getFixedData(),
	         mRegistrationManager->getMovingData(),
	         outDir.absolutePath(),
	         QStringList() << mActiveParameterFile);

}

void ElastixManager::executionFinishedSlot()
{
	bool ok = false;
	ssc::Transform3D mMf = mExecuter->getAffineResult_mMf(&ok);

	if (!ok)
		return;

	std::cout << "ElastixManager::executionFinishedSlot(), Linear Result mMf: \n" << mMf << std::endl;

	QString desc = QString("Image2Image [exe=%1][par=%2]")
		.arg(QFileInfo(this->getActiveExecutable()).fileName())
		.arg(QFileInfo(this->getActiveParameterFile()).fileName());

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

	ssc::ImagePtr movingImage = boost::shared_dynamic_cast<ssc::Image>(mRegistrationManager->getMovingData());
	ssc::ImagePtr raw = boost::shared_dynamic_cast<ssc::Image>(ssc::MetaImageReader().load(nonlinearVolumeFilename, nonlinearVolumeFilename));

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
