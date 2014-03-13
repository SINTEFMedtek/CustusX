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

#include "cxPatientData.h"

#include <QDomDocument>
#include <QFile>
#include <QDir>
#include <QTimer>
#include <QTextStream>
#include <QApplication>

#include "cxTime.h"
#include "cxMessageManager.h"
#include "cxUtilHelpers.h"
#include "cxCustomMetaImage.h"
#include "cxMesh.h"

#include "cxSettings.h"

#include <vtkPolyData.h>
#include <vtkPointData.h>

#include "cxDataManager.h"
#include "cxImage.h"
#include "cxTypeConversions.h"
#include "cxConfig.h"

namespace cx
{

/**given a root node, use the /-separated path to descend
 * into the root children recursively. Create elements if
 * necessary.
 *
 */
QDomElement getElementForced(QDomNode root, QString path)
{
	QStringList pathList = path.split("/");
	QDomElement current = root.toElement();

	if (current.isNull())
		return current;

	for (int i = 0; i < pathList.size(); ++i)
	{
		QDomElement next = current.namedItem(pathList[i]).toElement();

		if (next.isNull())
		{
			next = root.ownerDocument().createElement(pathList[i]);
			current.appendChild(next);
		}

		current = next;
	}

	return current;
}

PatientData::PatientData(DataServicePtr dataManager) : mDataManager(dataManager)
{
	connect(dataManager.get(), SIGNAL(clinicalApplicationChanged()), this, SLOT(clearPatient()));
}

PatientData::~PatientData()
{}

QString PatientData::getActivePatientFolder() const
{
	return mActivePatientFolder;
}

bool PatientData::isPatientValid() const
{
	return !mActivePatientFolder.isEmpty() && (mActivePatientFolder != this->getNullFolder());
}

QDomElement PatientData::getCurrentWorkingElement(QString path)
{
	return getElementForced(mWorkingDocument.documentElement(), path);
}

QDomDocument PatientData::getCurrentWorkingDocument()
{
	return mWorkingDocument;
}

QString PatientData::generateFilePath(QString folderName, QString ending)
{
	QString folder = this->getActivePatientFolder() + "/" +folderName + "/";
	QDir().mkpath(folder);
	QString format = timestampSecondsFormat();
	QString filename = QDateTime::currentDateTime().toString(format) + "." + ending;

	return folder+filename;
}

void PatientData::setActivePatient(const QString& activePatientFolder)
{
	if (activePatientFolder == mActivePatientFolder)
		return;

	mActivePatientFolder = activePatientFolder;

	messageManager()->sendInfo("Set Active Patient: " + mActivePatientFolder);

	emit patientChanged();
}

void PatientData::newPatient(QString choosenDir)
{
	this->clearPatient();
	createPatientFolders(choosenDir);
	this->setActivePatient(choosenDir);
}

/**Remove all data referring to the current patient from the system,
 * enabling us to load new patient data.
 */
void PatientData::clearPatient()
{
	mDataManager->clear();

	QString patientDatafolder = settings()->value("globalPatientDataFolder").toString();

	this->setActivePatient(this->getNullFolder());

	emit cleared();
}

QString PatientData::getNullFolder() const
{
	QString patientDatafolder = settings()->value("globalPatientDataFolder").toString();
	return patientDatafolder + "/NoPatient";
}

/**Parse command line and return --load <patient folder> folder,
 * if any.
 */
QString PatientData::getCommandLineStartupPatient()
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
void PatientData::startupLoadPatient()
{
	QString folder = this->getCommandLineStartupPatient();

	if (!folder.isEmpty())
	{
		messageManager()->sendInfo(QString("Startup Load [%1] from command line").arg(folder));
	}

	if (folder.isEmpty() && settings()->value("Automation/autoLoadRecentPatient").toBool())
	{
		folder = settings()->value("startup/lastPatient").toString();

		QDateTime lastSaveTime = QDateTime::fromString(settings()->value("startup/lastPatientSaveTime").toString(), timestampSecondsFormat());
		double minsSinceLastSave = lastSaveTime.secsTo(QDateTime::currentDateTime())/60;
		double autoLoadRecentPatientWithinHours = settings()->value("Automation/autoLoadRecentPatientWithinHours").toDouble();
		int allowedMinsSinceLastSave = autoLoadRecentPatientWithinHours*60;
		if (minsSinceLastSave > allowedMinsSinceLastSave) // if less than 8 hours, accept
		{
			messageManager()->sendInfo(
				QString("Startup Load: Ignored recent patient because %1 hours since last save, limit is %2")
				.arg(int(minsSinceLastSave/60))
				.arg(int(allowedMinsSinceLastSave/60)));
			folder = "";
		}

		if (!folder.isEmpty())
			messageManager()->sendInfo(QString("Startup Load [%1] as recent patient").arg(folder));
	}

	if (folder.isEmpty())
		return;

	this->loadPatient(folder);
}

void PatientData::loadPatient(QString choosenDir)
{
	this->clearPatient();
	if (choosenDir == QString::null)
		return; // On cancel

	QFile file(choosenDir + "/custusdoc.xml");
	if (file.open(QIODevice::ReadOnly))
	{
		QDomDocument doc;
		QString emsg;
		int eline, ecolumn;
		// Read the file
		if (!doc.setContent(&file, false, &emsg, &eline, &ecolumn))
		{
			messageManager()->sendError("Could not parse XML file :" + file.fileName() + " because: " + emsg + "");
		}
		else
		{
			//Read the xml
			this->readLoadDoc(doc, choosenDir);
		}
		file.close();
	}

	this->setActivePatient(choosenDir);
}

void PatientData::autoSave()
{
	if (settings()->value("Automation/autoSave").toBool())
		this->savePatient();
}

void PatientData::savePatient()
{

	if (mActivePatientFolder.isEmpty())
		return;

	//Gather all the information that needs to be saved
	QDomDocument doc;
	this->generateSaveDoc(doc);
	mWorkingDocument = doc;
	emit isSaving(); // give all listeners a chance to add to the document

	QFile file(mActivePatientFolder + "/custusdoc.xml");
	if (file.open(QIODevice::WriteOnly | QIODevice::Truncate))
	{
		QTextStream stream(&file);
		stream << doc.toString(4);
		file.close();
	}
	else
	{
		messageManager()->sendError("Could not open " + file.fileName() + " Error: " + file.errorString());
	}

	// save position transforms into the mhd files.
	// This hack ensures data files can be used in external programs without an explicit export.
	DataManager::ImagesMap images = mDataManager->getImages();
	for (DataManager::ImagesMap::iterator iter = images.begin(); iter != images.end(); ++iter)
	{
		CustomMetaImagePtr customReader = CustomMetaImage::create(
						mActivePatientFolder + "/" + iter->second->getFilename());
		customReader->setTransform(iter->second->get_rMd());
	}

	//Write the data to file, fx modified images... etc...
	//TODO Implement when we know what we want to save here...

	mWorkingDocument = QDomDocument();

	messageManager()->sendInfo("Saved patient " + mActivePatientFolder);
}

/** Writes settings info describing the patient name and current time.
  * Used for auto load.
  */
void PatientData::writeRecentPatientData()
{
	settings()->setValue("startup/lastPatient", mActivePatientFolder);
	settings()->setValue("startup/lastPatientSaveTime", QDateTime::currentDateTime().toString(timestampSecondsFormat()));
}


void PatientData::exportPatient(bool niftiFormat)
{
	QString targetFolder = mActivePatientFolder + "/Export/"
					+ QDateTime::currentDateTime().toString(timestampSecondsFormat());

	DataManager::ImagesMap images = mDataManager->getImages();
	for (DataManager::ImagesMap::iterator iter = images.begin(); iter != images.end(); ++iter)
	{
		mDataManager->saveImage(iter->second, targetFolder);
	}

	DataManager::MeshMap meshes = mDataManager->getMeshes();
	for (DataManager::MeshMap::iterator iter = meshes.begin(); iter != meshes.end(); ++iter)
	{
		MeshPtr mesh = iter->second;

		Transform3D rMd = mesh->get_rMd();
		if (niftiFormat)
		{
			rMd = rMd * createTransformRotateZ(M_PI); // convert back to NIFTI format
			messageManager()->sendInfo("Nifti export: rotated data " + mesh->getName() + " 180* around Z-axis.");
		}

		vtkPolyDataPtr poly = mesh->getTransformedPolyData(rMd);
		// create a copy with the SAME UID as the original. Do not load this one into the datamanager!
		mesh = mDataManager->createMesh(poly, mesh->getUid(), mesh->getName(), "Images");
		mDataManager->saveMesh(mesh, targetFolder);
	}

	messageManager()->sendInfo("Exported patient data to " + targetFolder + ".");
}

bool PatientData::copyFile(QString source, QString dest, QString &infoText)
{
	if (source == dest)
		return true;

	QFileInfo info(dest);

	if (info.exists())
	{
		QString text = "File already exists: " + dest + ", copy skipped.";
		infoText = "<font color=orange>" + text + "</font><br>";
		messageManager()->sendWarning(text);
		return true;
	}

	QDir().mkpath(info.path());

	QFile toFile(dest);
	QFile(source).copy(toFile.fileName());
	if (!toFile.flush())
	{
		QString text = "Failed to copy file: " + source;
		messageManager()->sendWarning(text);
		infoText = "<font color=red>" + text + "</font><br>";
		return false;
	}
	if (!toFile.exists())
	{
		QString text = "File not copied: " + source;
		messageManager()->sendWarning(text);
		infoText = "<font color=red>" + text + "</font><br>";
		return false;
	}

	messageManager()->sendInfo("Copied " + source + " -> " + dest);

	return true;
}

//bool PatientData::copyAllSimilarFiles(QString fileName, QString destFolder, QString &infoText)
//{
//	QDir sourceFolder(QFileInfo(fileName).path());
//	QStringList filter;
//	filter << QFileInfo(fileName).completeBaseName() + ".*";
//	QStringList sourceFiles = sourceFolder.entryList(filter, QDir::Files);

//	for (int i = 0; i < sourceFiles.size(); ++i)
//	{
//		QString sourceFile = sourceFolder.path() + "/" + sourceFiles[i];
//		QString destFile = destFolder + "/" + QFileInfo(sourceFiles[i]).fileName();
//		QString text;
//		this->copyFile(sourceFile, destFile, text);
//		infoText.append(text);
//	}

//	return true;
//}

DataPtr PatientData::importData(QString fileName, QString &infoText)
{
	if (fileName.isEmpty())
	{
		QString text = "Import canceled";
		messageManager()->sendInfo(text);
		infoText = "<font color=red>" + text + "</font>";
		return DataPtr();
	}

//	QString patientsImageFolder = mActivePatientFolder + "/Images/";

	QFileInfo fileInfo(fileName);
	QString fileType = fileInfo.suffix();
//	QString pathToNewFile = patientsImageFolder + fileInfo.fileName();
	QFile fromFile(fileName);
	QString strippedFilename = changeExtension(fileInfo.fileName(), "");
	QString uid = strippedFilename + "_" + QDateTime::currentDateTime().toString(timestampSecondsFormat());

	if (mDataManager->getData(uid))
	{
		QString text = "Data with uid " + uid + " already exists. Import canceled.";
		messageManager()->sendWarning(text);
		infoText = "<font color=red>" + text + "</font>";
		return DataPtr();
	}

	// Read files before copy
	DataPtr data = mDataManager->loadData(uid, fileName);
	if (!data)
	{
		QString text = "Error with data file: " + fileName + " Import canceled.";
		messageManager()->sendWarning(text);
		infoText = "<font color=red>" + text + "</font>";
		return DataPtr();
	}
	data->setAcquisitionTime(QDateTime::currentDateTime());

	data->setShadingOn(true);

//	QDir patientDataDir(mActivePatientFolder);

//	data->setFilePath(patientDataDir.relativeFilePath(pathToNewFile)); // Update file path

	mDataManager->saveData(data, mActivePatientFolder);
//	this->copyAllSimilarFiles(fileName, patientsImageFolder, infoText);

	// remove redundant line breaks
	infoText = infoText.split("<br>", QString::SkipEmptyParts).join("<br>");

	return data;
}

void PatientData::removeData(QString uid)
{
	mDataManager->removeData(uid, this->getActivePatientFolder());
}

void PatientData::createPatientFolders(QString choosenDir)
{
	if (!choosenDir.endsWith(".cx3"))
		choosenDir.append(".cx3");

	messageManager()->sendInfo("Selected a patient to work with.");

	// Create folders
	if (!QDir().exists(choosenDir))
	{
		QDir().mkdir(choosenDir);
		messageManager()->sendInfo("Made a new patient folder: " + choosenDir);
	}

	QString newDir = choosenDir;
	newDir.append("/Images");
	if (!QDir().exists(newDir))
	{
		QDir().mkdir(newDir);
		messageManager()->sendInfo("Made a new image folder: " + newDir);
	}

	newDir = choosenDir;
	newDir.append("/Logs");
	if (!QDir().exists(newDir))
	{
		QDir().mkdir(newDir);
		messageManager()->sendInfo("Made a new logging folder: " + newDir);
	}

	newDir = choosenDir;
	newDir.append("/US_Acq");
	if (!QDir().exists(newDir))
	{
		QDir().mkdir(newDir);
		messageManager()->sendInfo("Made a new ultrasound folder: " + newDir);
	}

	this->savePatient();
}

QString PatientData::getVersionName()
{
    return QString("%1").arg(CustusX3_VERSION_STRING);
}

/**
 * Xml version 1.0: Knows about the nodes: \n
 * \<managers\> \n
 *   \<datamanager\> \n
 *     \<image\> \n
 *        \<uid\> //an images unique id \n
 *        \<name\> //an images name \n
 *        \<transferfunctions\> //an images transferefunction \n
 *            \<alpha\> //a transferefunctions alpha values \n
 *            \<color\> //a transferefunctions color values
 */
/**
 * Xml version 2.0: Knows about the nodes: \n
 * \<patient\> \n
 *  \<active_patient\> //relative path to this patients folder \n
 *  \<managers\> \n
 *     \<datamanager\> \n
 *       \<image\> \n
 *         \<uid\> //an images unique id \n
 *         \<name\> //an images name \n
 *         \<transferfunctions\> //an images transferefunction \n
 *            \<alpha\> //a transferefunctions alpha values \n
 *            \<color\> //a transferefunctions color values \n
 */
void PatientData::generateSaveDoc(QDomDocument& doc)
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

	QDomElement managerNode = doc.createElement("managers");
	patientNode.appendChild(managerNode);

	mDataManager->addXml(managerNode);
}

void PatientData::readLoadDoc(QDomDocument& doc, QString patientFolder)
{
	mWorkingDocument = doc;
	//Get all the nodes
	QDomNode patientNode = doc.namedItem("patient");
	QDomNode managerNode = patientNode.namedItem("managers");

	//Evaluate the xml nodes and load what's needed
	QDomNode dataManagerNode = managerNode.namedItem("datamanager");

	if (!dataManagerNode.isNull())
	{
		mDataManager->parseXml(dataManagerNode, patientFolder);
	}

	emit
	isLoading();

	mWorkingDocument = QDomDocument();
}

} // namespace cx

