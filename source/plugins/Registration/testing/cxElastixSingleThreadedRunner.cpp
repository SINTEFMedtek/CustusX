/*
 * cxElastixSingleThreadedRunner.cpp
 *
 *  Created on: Oct 8, 2012
 *      Author: christiana
 */

#include "cxElastixSingleThreadedRunner.h"
#include "cxElastixExecuter.h"
#include "sscXmlOptionItem.h"
#include <QDir>
#include "cxDataLocations.h"
#include <qapplication.h>
#include "sscTime.h"

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
    QString preset,
    ssc::Transform3D* result)
{
	mCompleted = false;
	QDir dir(cx::DataLocations::getRootConfigPath() + "/elastix");
	ssc::XmlOptionFile mOptions = ssc::XmlOptionFile(DataLocations::getXmlSettingsFile(), "CustusX").descend("elastix");
	ssc::XmlOptionFile node = mOptions.descend("preset", "name", preset);
	QString mActiveExecutable = node.getElement().attribute("executable");
	QString mActiveParameterFile0 = dir.filePath(node.getElement().attribute("parameterFile0"));
	QString mActiveParameterFile1 = dir.filePath(node.getElement().attribute("parameterFile1"));

    QStringList parameterFiles;
    if (QFileInfo(mActiveParameterFile0).exists() && QFileInfo(mActiveParameterFile0).isFile())
    	parameterFiles << mActiveParameterFile0;
    if (QFileInfo(mActiveParameterFile1).exists() && QFileInfo(mActiveParameterFile1).isFile())
    	parameterFiles  << mActiveParameterFile1;

	QString outPath = QDir::homePath() + "/Patients/testing/elastix/" + QDateTime::currentDateTime().toString(ssc::timestampMilliSecondsFormat() + "/");

	mExecuter->setDisplayProcessMessages(false);
	mExecuter->setInput(mActiveExecutable,
	         fixed,
	         moving,
	         outPath,
	         parameterFiles);

	std::cout << "ElastixSingleThreadedRunner::registerLinear()" << std::endl;

	while (!mCompleted)
		qApp->processEvents();

	*result = m_mMf;
	return true;
}

void ElastixSingleThreadedRunner::preprocessExecuter()
{
	std::cout << "ElastixSingleThreadedRunner::preprocessExecuter()" << std::endl;
}

void ElastixSingleThreadedRunner::executionFinishedSlot()
{
	std::cout << "ElastixSingleThreadedRunner::executionFinishedSlot()" << std::endl;

	bool ok = false;
	m_mMf = mExecuter->getAffineResult_mMf(&ok);

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
	mCompleted = true;
}



}
