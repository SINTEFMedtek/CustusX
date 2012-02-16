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

namespace cx
{

ElastixManager::ElastixManager(RegistrationManagerPtr regManager) : mRegistrationManager(regManager)
{
	mOptions = ssc::XmlOptionFile(DataLocations::getXmlSettingsFile(), "CustusX").descend("elastix");

	mActiveExecutable = mOptions.getElement().attribute("executable");
	mActiveParameterFile = mOptions.getElement().attribute("parameterFile");
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
	SSC_LOG("exec");
//	QDir folder(cx::DataLocations::getRootConfigPath() + "/elastix");
	QString timestamp = QDateTime::currentDateTime().toString(ssc::timestampSecondsFormat());
	QDir outDir(patientService()->getPatientData()->getActivePatientFolder()+"/elastix/"+timestamp);

	ElastixExecuter executer;
	executer.run(mActiveExecutable,
	         boost::shared_dynamic_cast<ssc::Image>(mRegistrationManager->getFixedData()),
	         boost::shared_dynamic_cast<ssc::Image>(mRegistrationManager->getMovingData()),
	         outDir.absolutePath(),
	         QStringList() << mActiveParameterFile);

}



} /* namespace cx */
