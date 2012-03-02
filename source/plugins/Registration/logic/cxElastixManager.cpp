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

	mDisplayProcessMessages = ssc::BoolDataAdapterXml::initialize("displayProcessMessages",
		"Show Messages",
		"Display messages from the running registration process in CustusX",
		false,
		mOptions.getElement());

	mCurrentPreset = ssc::StringDataAdapterXml::initialize("currentPreset", "Preset", "Current Elastix Preset", "Select Preset...", QStringList(), mOptions.getElement());
	connect(mCurrentPreset.get(), SIGNAL(changed()), this, SLOT(currentPresetChangedSlot()));

	mExecuter.reset(new ElastixExecuter());
	connect(mExecuter.get(), SIGNAL(finished()), this, SLOT(executionFinishedSlot()));

	this->currentPresetChangedSlot();
}

/**Called when the current preset changes. Save to settings and reload dependent values
 *
 */
void ElastixManager::currentPresetChangedSlot()
{
	SSC_LOG("ElastixManager::currentPresetChangedSlot()");

	this->reloadPresets();

	QDir dir(cx::DataLocations::getRootConfigPath() + "/elastix");
	ssc::XmlOptionFile node = mOptions.descend("preset", "name", mCurrentPreset->getValue());
	mActiveExecutable = node.getElement().attribute("executable");
	mActiveParameterFile0 = dir.filePath(node.getElement().attribute("parameterFile0"));
	mActiveParameterFile1 = dir.filePath(node.getElement().attribute("parameterFile1"));
	emit elastixChanged();
}

ssc::StringDataAdapterPtr ElastixManager::getCurrentPreset()
{
	return mCurrentPreset;
}

/**Remove the currently selected preset. Reload.
 *
 */
void ElastixManager::removeCurrentPreset()
{
	ssc::XmlOptionFile node = mOptions.descend("preset", "name", mCurrentPreset->getValue());
	node.deleteNode();
	this->reloadPresets();
	mCurrentPreset->setValue("Select Preset...");
}

/**Read presets anew and update the current.
 *
 */
void ElastixManager::reloadPresets()
{
	SSC_LOG("ElastixManager::reloadPresets()");
	QStringList presets;
	presets << "Select Preset...";

	QDomNodeList presetNodeList = mOptions.getElement().elementsByTagName("preset");
	for (int i = 0; i < presetNodeList.count(); ++i)
	{
		presets << presetNodeList.item(i).toElement().attribute("name");
	}
	presets.removeDuplicates();

	mCurrentPreset->blockSignals(true);
	mCurrentPreset->setValueRange(presets);
	mCurrentPreset->blockSignals(false);
}


void ElastixManager::saveCurrentPreset(QString name)
{
	ssc::XmlOptionFile node = mOptions.descend("preset", "name", name);
	node.getElement().setAttribute("executable", mActiveExecutable);
	node.getElement().setAttribute("parameterFile0", QFileInfo(mActiveParameterFile0).fileName());
	node.getElement().setAttribute("parameterFile1", QFileInfo(mActiveParameterFile1).fileName());
	mCurrentPreset->setValue(name);
}


ElastixManager::~ElastixManager()
{
}

void ElastixManager::setActiveParameterFile0(QString filename)
{
	mActiveParameterFile0 = filename;

//	mOptions.getElement().setAttribute("parameterFile", mActiveParameterFile0);
	emit elastixChanged();
}

QString ElastixManager::getActiveParameterFile0() const
{
	return mActiveParameterFile0;
}

void ElastixManager::setActiveParameterFile1(QString filename)
{
	mActiveParameterFile1 = filename;
	emit elastixChanged();
}

QString ElastixManager::getActiveParameterFile1() const
{
	return mActiveParameterFile1;
}

void ElastixManager::setActiveExecutable(QString filename)
{
	mActiveExecutable = filename;
//	mOptions.getElement().setAttribute("executable", mActiveExecutable);
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
	         QStringList() << mActiveParameterFile0 << mActiveParameterFile1);

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
		.arg(QFileInfo(this->getActiveParameterFile0()).fileName());

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
