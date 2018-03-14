/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxSessionStorageServiceImpl.h"
#include "cxSettings.h"
#include "cxReporter.h"
#include <QTextStream>
#include "cxTime.h"
#include <QDir>
#include "cxConfig.h"
#include "cxFileHelpers.h"
#include "cxDataLocations.h"
#include <QApplication>
#include <QTimer>
#include "cxLogger.h"
#include "cxProfile.h"
#include "cxOrderedQDomDocument.h"
#include "cxXmlFileHandler.h"


namespace cx
{

SessionStorageServiceImpl::SessionStorageServiceImpl(ctkPluginContext *context)
{
	this->clearCache();
	mActivePatientFolder = this->getNoPatientFolder();
	connect(this, &SessionStorageServiceImpl::sessionChanged, this, &SessionStorageServiceImpl::onSessionChanged);

	QTimer::singleShot(100, this, SLOT(startupLoadPatient())); // make sure this is called after application state change
}

SessionStorageServiceImpl::~SessionStorageServiceImpl()
{
	this->clearCache();
}

bool SessionStorageServiceImpl::isNull() const
{
	return false;
}

void SessionStorageServiceImpl::load(QString dir)
{
	bool valid = this->isValidSessionFolder(dir);
	bool exists = this->folderExists(dir);

	if (valid)
	{
		this->loadSession(dir);
	}
	else if (!exists)
	{
		this->initializeNewSession(dir);
	}
	else
	{
		reportError(QString("Failed to initialize session in existing folder with no valid session: %1").arg(dir));
	}
}

QString SessionStorageServiceImpl::getNoPatientFolder() const
{
	return DataLocations::getCachePath() + "/NoPatient";
}


bool SessionStorageServiceImpl::isValidSessionFolder(QString dir) const
{
	QString filename = QDir(dir).absoluteFilePath(this->getXmlFileName());
	return QFileInfo::exists(filename);
}

bool SessionStorageServiceImpl::folderExists(QString dir) const
{
	return QFileInfo(dir).exists() && QFileInfo(dir).isDir();
}

void SessionStorageServiceImpl::loadSession(QString dir)
{
	if (this->isActivePatient(dir))
		return;
	this->loadPatientSilent(dir);
	this->reportActivePatient();
	this->writeRecentPatientData();
}

void SessionStorageServiceImpl::initializeNewSession(QString dir)
{
	this->clearPatientSilent();
	dir = this->convertToValidFolderName(dir);
	this->createPatientFolders(dir);
	this->setActivePatient(dir);
	this->save();
	this->reportActivePatient();
	this->writeRecentPatientData();
}

QString SessionStorageServiceImpl::getXmlFileName() const
{
	return "custusdoc.xml";
}

void SessionStorageServiceImpl::save()
{
	if (!this->isValid())
		return;

	//Gather all the information that needs to be saved
	OrderedQDomDocument doc;
	this->generateSaveDoc(doc.doc());

	QDomElement element = doc.doc().documentElement();
	emit isSaving(element); // give all listeners a chance to add to the document

	QString filename = QDir(mActivePatientFolder).absoluteFilePath(this->getXmlFileName());
	XmlFileHandler::writeXmlFile(doc.doc(), filename);
	report("Saved patient " + mActivePatientFolder);
}

void SessionStorageServiceImpl::clear()
{
	this->clearPatientSilent();
	this->reportActivePatient();
	this->writeRecentPatientData();
}

bool SessionStorageServiceImpl::isValid() const
{
	return !mActivePatientFolder.isEmpty() &&
			(mActivePatientFolder != this->getNoPatientFolder());
}

QString SessionStorageServiceImpl::getRootFolder() const
{
	return mActivePatientFolder;
}

void SessionStorageServiceImpl::reportActivePatient()
{
	report("Set Active Patient: " + mActivePatientFolder);
}

void SessionStorageServiceImpl::setActivePatient(const QString& activePatientFolder)
{
	if (activePatientFolder == mActivePatientFolder)
		return;

	mActivePatientFolder = activePatientFolder;

	emit sessionChanged();
}

void SessionStorageServiceImpl::onSessionChanged()
{
	reporter()->setLoggingFolder(this->getSubFolder("Logs/"));
}

void SessionStorageServiceImpl::clearPatientSilent()
{
	this->setActivePatient(this->getNoPatientFolder());
	emit cleared();
}

bool SessionStorageServiceImpl::isActivePatient(QString patient) const
{
	return (patient == mActivePatientFolder);
}

void SessionStorageServiceImpl::loadPatientSilent(QString choosenDir)
{
	if (this->isActivePatient(choosenDir))
		return;
	this->clearPatientSilent();
	if (choosenDir == QString::null)
		return; // On cancel

	QString filename = QDir(choosenDir).absoluteFilePath(this->getXmlFileName());
	QDomDocument doc = XmlFileHandler::readXmlFile(filename);
	mActivePatientFolder = choosenDir; // must set path before emitting isLoading()

	if (!doc.isNull())
	{
		//Read the xml
		QDomElement element = doc.documentElement();
		emit isLoading(element);
		emit isLoadingSecond(element);
	}

	emit sessionChanged();
}

/** Writes settings info describing the patient name and current time.
  * Used for auto load.
  */
void SessionStorageServiceImpl::writeRecentPatientData()
{
	settings()->setValue("startup/lastPatient", mActivePatientFolder);
	settings()->setValue("startup/lastPatientSaveTime", QDateTime::currentDateTime().toString(timestampSecondsFormat()));
}

void SessionStorageServiceImpl::generateSaveDoc(QDomDocument& doc)
{
	doc.appendChild(doc.createProcessingInstruction("xml version =", "'1.0'"));

	QDomElement patientNode = doc.createElement("patient");

	// note: all nodes must be below <patient>. XML requires only one root node per file.
	QDomElement versionName = doc.createElement("version_name");
	versionName.appendChild(doc.createTextNode(this->getVersionName()));
	patientNode.appendChild(versionName);

	QDomElement activePatientNode = doc.createElement("active_patient");
	activePatientNode.appendChild(doc.createTextNode(mActivePatientFolder.toStdString().c_str()));
	patientNode.appendChild(activePatientNode);
	doc.appendChild(patientNode);
}

QString SessionStorageServiceImpl::convertToValidFolderName(QString dir) const
{
	if (!dir.endsWith(".cx3"))
		dir.append(".cx3");
	return dir;
}

void SessionStorageServiceImpl::createPatientFolders(QString dir)
{
	QDir().mkpath(dir);
	QDir().mkpath(dir+"/Images");
	QDir().mkpath(dir+"/Logs");
	QDir().mkpath(dir+"/US_Acq");
}

QString SessionStorageServiceImpl::getVersionName()
{
	return QString("%1").arg(CustusX_VERSION_STRING);
}

void SessionStorageServiceImpl::clearCache()
{
//	std::cout << "DataLocations::getCachePath() " << DataLocations::getCachePath() << std::endl;
	// clear the global cache used by app
	removeNonemptyDirRecursively(DataLocations::getCachePath());
}

/**Parse command line and return --load <patient folder> folder,
 * if any.
 */
QString SessionStorageServiceImpl::getCommandLineStartupPatient()
{
	int doLoad = QApplication::arguments().indexOf("--load");
	if (doLoad < 0)
		return "";
	if (doLoad + 1 >= QApplication::arguments().size())
		return "";

	QString folder = QApplication::arguments()[doLoad + 1];

	return folder;
}

/**Parse the command line and load a patient if the switch --patient is found
 */
void SessionStorageServiceImpl::startupLoadPatient()
{
	QString folder = this->getCommandLineStartupPatient();

	if (!folder.isEmpty())
	{
		report(QString("Startup Load [%1] from command line").arg(folder));
	}

	if (folder.isEmpty() && settings()->value("Automation/autoLoadRecentPatient").toBool())
	{
		folder = settings()->value("startup/lastPatient").toString();
        if(this->isValidSessionFolder(folder))
        {
            QDateTime lastSaveTime = QDateTime::fromString(settings()->value("startup/lastPatientSaveTime").toString(), timestampSecondsFormat());
            double minsSinceLastSave = lastSaveTime.secsTo(QDateTime::currentDateTime())/60;
            double autoLoadRecentPatientWithinHours = settings()->value("Automation/autoLoadRecentPatientWithinHours").toDouble();
            int allowedMinsSinceLastSave = autoLoadRecentPatientWithinHours*60;
            if (minsSinceLastSave > allowedMinsSinceLastSave) // if less than 8 hours, accept
            {
                report(
                    QString("Startup Load: Ignored recent patient because %1 hours since last save, limit is %2")
                    .arg(int(minsSinceLastSave/60))
                    .arg(int(allowedMinsSinceLastSave/60)));
                folder = "";
            }
            if (!folder.isEmpty())
                report(QString("Startup Load [%1] as recent patient").arg(folder));
        }
        else
        {
            report("Startup Load: Ignored recent patient because it is not valid anymore");
            folder = "";
        }

	}

	if (folder.isEmpty())
		return;

	this->load(folder);
}

} // namespace cx
