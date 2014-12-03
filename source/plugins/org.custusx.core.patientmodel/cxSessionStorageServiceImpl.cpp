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

#include "cxSessionStorageServiceImpl.h"
#include "cxSettings.h"
#include "cxReporter.h"
#include <QTextStream>
#include "cxTime.h"
#include <QDir>
#include "cxConfig.h"


namespace cx
{

SessionStorageServiceImpl::SessionStorageServiceImpl()
{

}

SessionStorageServiceImpl::~SessionStorageServiceImpl()
{

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
	if (mActivePatientFolder.isEmpty())
		return;

	//Gather all the information that needs to be saved
	QDomDocument doc;
	this->generateSaveDoc(doc);

	QDomElement element = doc.documentElement();
	emit isSaving(element); // give all listeners a chance to add to the document

	QString filename = QDir(mActivePatientFolder).absoluteFilePath(this->getXmlFileName());
	this->writeXmlFile(doc, filename);

	report("Saved patient " + mActivePatientFolder);
	this->writeRecentPatientData();
}

void SessionStorageServiceImpl::writeXmlFile(QDomDocument doc, QString filename)
{
	QFile file(filename);
	if (file.open(QIODevice::WriteOnly | QIODevice::Truncate))
	{
		QTextStream stream(&file);
		stream << doc.toString(4);
		file.close();
	}
	else
	{
		reportError("Could not open " + file.fileName() + " Error: " + file.errorString());
	}
}

void SessionStorageServiceImpl::clear()
{
	this->clearPatientSilent();
	this->reportActivePatient();
	this->writeRecentPatientData();
}

bool SessionStorageServiceImpl::isValid() const
{
	return !mActivePatientFolder.isEmpty() && (mActivePatientFolder != this->getNullFolder());
}

QString SessionStorageServiceImpl::getRootFolder() const
{
	return mActivePatientFolder;
}

QString SessionStorageServiceImpl::getNullFolder() const
{
	QString patientDatafolder = settings()->value("globalPatientDataFolder").toString();
	return patientDatafolder + "/NoPatient";
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

void SessionStorageServiceImpl::clearPatientSilent()
{
	this->setActivePatient(this->getNullFolder());
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
	QDomDocument doc = this->readXmlFile(filename);

	mActivePatientFolder = choosenDir; // must set path before emitting isLoading()

	if (!doc.isNull())
	{
		//Read the xml
		QDomElement element = doc.documentElement();
		emit isLoading(element);
	}

	emit sessionChanged();
}

QDomDocument SessionStorageServiceImpl::readXmlFile(QString filename)
{
	QDomDocument retval;
	QFile file(filename);
	if (!file.open(QIODevice::ReadOnly))
	{
		reportError("Could not open XML file :" + file.fileName() + ".");
		return QDomDocument();
	}

	QString emsg;
	int eline, ecolumn;
	// Read the file
	if (!retval.setContent(&file, false, &emsg, &eline, &ecolumn))
	{
		reportError("Could not parse XML file :" + file.fileName() + " because: " + emsg + "");
		return QDomDocument();
	}

	return retval;
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


} // namespace cx
