/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxDICOMReader.h"

#include <QDir>
#include <QProgressDialog>
#include <vtkImageData.h>
#include <ctkDICOMDatabase.h>
#include <ctkDICOMIndexer.h>
#include "cxImage.h"
#include "cxPatientModelService.h"
#include "cxLogger.h"
#include "cxDicomConverter.h"
#include "cxDicomImageReader.h"
#include "cxReporter.h"
#include "cxProfile.h"

//dcmtk includes
#include "dcmtk/config/osconfig.h"
#include "dcmtk/oflog/nullap.h"

namespace cx
{
DICOMReader::DICOMReader(PatientModelServicePtr patientModelService) :
	FileReaderWriterImplService("DICOMReader" ,Image::getTypeName(), "", "dcm *", patientModelService)
{
	this->stopDCMTKMessages();
}

// This function is used for checking all readers if they can read a file,
// so output should only be used for debug.
bool DICOMReader::canRead(const QString &type, const QString &filename)
{
	if(QFileInfo(filename).isDir())
		return this->canReadDir(filename, true);

	return this->canReadFile(filename);
}

bool DICOMReader::canReadDir(QString dirname, bool checkSubDirs)
{
	QDir dir(dirname);
	QStringList files = dir.entryList(QDir::NoDotAndDotDot | QDir::AllEntries);
	for(int i = 0; i < files.size(); ++i)
	{
		QString fullPath = dirname+"/"+files[i];
		if(this->canReadFile(fullPath))
			return true;
		else if(checkSubDirs && QFileInfo(fullPath).isDir())
		{
			if(this->canReadDir(fullPath, checkSubDirs))
				return true;
		}
	}
	return false;
}

bool DICOMReader::canReadFile(QString filename)
{
	if(QFileInfo(filename).isDir())
	{
		return false; //Don't look into subdirs
	}

	QFile file(filename);
	bool opened = file.open(QIODevice::ReadOnly);
	if(!opened)
		return false;

	// A DICOM file should have the characters "DICM" at position 0x80
	bool success = file.seek(0x80);
	if(!success)
	{
		//CX_LOG_WARNING() << "DICOMReader: File isn't large enough to be DICOM: " << filename;
		return false;
	}

	char buf[4];
	int numReadChar = file.peek(buf, sizeof(buf));
	if (numReadChar != sizeof(buf))
	{
		//CX_LOG_WARNING() << "DICOMReader: Cannot read from file: " << filename;
		return false;
	}
	if (buf[0] != 'D' || buf[1] != 'I' || buf[2] != 'C' || buf[3] != 'M')
	{
		//CX_LOG_WARNING() << "DICOMReader: File isn't DICOM: " << filename;
		return false;
	}
	else
		return true;
}

QString DICOMReader::canReadDataType() const
{
	return Image::getTypeName();
}

bool DICOMReader::readInto(DataPtr data, QString filename)
{
	return this->readInto(boost::dynamic_pointer_cast<Image>(data), filename);
}

bool DICOMReader::readInto(ImagePtr image, QString filename)
{
	if (!image)
		return false;
	vtkImageDataPtr raw = this->loadVtkImageData(filename);
	if(!raw)
		return false;
	image->setVtkImageData(raw);
	return true;
}

DataPtr DICOMReader::read(const QString& uid, const QString& filename)
{
	ImagePtr image(new Image(uid, vtkImageDataPtr()));
	this->readInto(image, filename);

	DataPtr retval = image;
	return retval;
}

std::vector<DataPtr> DICOMReader::read(const QString &filename)
{
	std::vector<DataPtr> retval;
	std::vector<ImagePtr> images = importSeries(filename, false);

	for(int i = 0; i < images.size(); ++i)
		retval.push_back(images[i]);

//	CX_LOG_DEBUG() << "Found " << retval.size() << " DICOM series";
	return retval;
}

vtkImageDataPtr DICOMReader::loadVtkImageData(QString filename)
{
	bool readBestSeries = true;
	std::vector<ImagePtr> images = importSeries(filename, readBestSeries);
	return images[0]->getBaseVtkImageData();
}

QString DICOMReader::canWriteDataType() const
{
	return "";
}

bool DICOMReader::canWrite(const QString &type, const QString &filename) const
{
	return false;
}

//Copied from DicomWidget getDICOMDatabaseDirectory(), setupDatabaseDirectory(), and deleteDICOMDB
QString DICOMReader::getDICOMDatabaseDirectory()
{
	QString databaseDirectory = profile()->getSettingsPath() + "/DICOMDatabase";

	return databaseDirectory;
}

void DICOMReader::setupDatabaseDirectory()
{
	QString databaseDirectory = this->getDICOMDatabaseDirectory();

	QDir qdir(databaseDirectory);
	if ( !qdir.exists(databaseDirectory) )
	{
		if ( !qdir.mkpath(databaseDirectory) )
		{
			CX_LOG_CHANNEL_ERROR("dicom") << "Could not create database directory \"" << databaseDirectory;
		}
	}

	CX_LOG_CHANNEL_INFO("dicom") << "DatabaseDirectory set to: " << databaseDirectory;
}

//TODO: Delete database after use
//void DICOMReader::deleteDICOMDB()
//{
//	CX_LOG_CHANNEL_INFO("dicom") << "Deleting DICOM database: " << this->getDICOMDatabaseDirectory();
//	bool autoDeleteDICOMDB = settings()->value("Automation/autoDeleteDICOMDatabase").toBool();
//	if(autoDeleteDICOMDB)
//	{
//		ctkDICOMDatabase* database = this->getDatabase();
//		if(database)
//		{
//			QStringList patients = database->patients();
//			foreach(QString patient , patients)
//			{
//				this->getDatabase()->removePatient(patient);
//			}
//		}
//	}
//}


//Copied from DicomWidget::importSeries
//Also copied DicomConverter and DicomImageReader files from the dicom plugin
std::vector<ImagePtr> DICOMReader::importSeries(QString fileName, bool readBestSeries)
{
	//Turn off Qt messages temporarily
	CX_LOG_INFO() << "stopQtMessages while reading DICOM files";
//	reporter()->stopQtMessages();

	ctkDICOMDatabasePtr database = ctkDICOMDatabasePtr(new ctkDICOMDatabase);
//	database->openDatabase(":memory:");//Looks like the memory option no longer works. ctkDICOMIndexerPrivateWorker::start() requires a file. TODO: Create CTK issue report?
	this->setupDatabaseDirectory();

	QString databaseFileName = this->getDICOMDatabaseDirectory() + QString("/ctkDICOM.sql");

	database->openDatabase(databaseFileName);//This seems to work
	CX_LOG_DEBUG() << "this->getDICOMDatabaseDirectory(): " << this->getDICOMDatabaseDirectory();
	CX_LOG_DEBUG() << "database->isInMemory(): " << database->isInMemory();
	CX_LOG_DEBUG() << "database->isOpen(): " << database->isOpen();

	QFileInfo dir = QFileInfo(fileName);
	QString folder = dir.absolutePath();
	if(dir.isDir())
		folder = fileName;

	QProgressDialog progress("Reading DICOM series...", "Cancel", 0, 0);
	progress.setWindowModality(Qt::WindowModal);
	progress.setMinimumDuration(0);
	progress.setValue(0);

	QStringList dicomFolders = this->findAllSubfoldersWithDicomFiles(folder, progress);

	progress.setLabelText("Reading DICOM series...");
	progress.setMaximum(dicomFolders.size());

	for(int i = 0; i < dicomFolders.size(); ++i)
	{
		progress.setValue(i);
		if (progress.wasCanceled())
			break;
		addFolderToDicomDatabase(database,dicomFolders[i]);
	}


	QStringList patients = database.data()->patients();
	CX_LOG_DEBUG() << "DICOMReader::importSeries patients: " << patients.join(", ");

	std::vector<ImagePtr> retval;
	if(readBestSeries)
		retval = importBestSeries(database);
	else
		retval = importAllSeries(database, progress);

	database->closeDatabase();

	//Turn Qt messages back on
//	reporter()->startQtMessages();
	CX_LOG_INFO() << "DICOM files read - startQtMessages";
	return retval;
}


QStringList DICOMReader::findAllSubfoldersWithDicomFiles(QString folder, QProgressDialog &progress)
{
	CX_LOG_INFO() << "Finding all subfolders in: " << folder;
	QStringList subDirs = findAllSubDirs(folder);

	progress.setLabelText("Searcing all subdirectories for DICOM series: "+folder);
	progress.setMaximum(subDirs.size());
	QStringList retval;
	for(int i = 0; i < subDirs.size(); ++i)
	{
		progress.setValue(i);
		if (progress.wasCanceled())
			break;
		if(this->canReadDir(subDirs[i], false))
			retval << subDirs[i];
	}
	return retval;
}

QStringList DICOMReader::findAllSubDirs(QString folder)
{
	if(!QFileInfo(folder).isDir())
		return QStringList();

	QStringList allSubDirs;

	QDir dir(folder);
	QStringList files = dir.entryList(QDir::NoDotAndDotDot | QDir::Dirs);
	for(int i = 0; i < files.size(); ++i)
	{
		QString fullPath = folder+"/"+files[i];
		QStringList subDirs = findAllSubDirs(fullPath);
		allSubDirs << subDirs;
	}
	allSubDirs << folder;

	return allSubDirs;
}

void DICOMReader::addFolderToDicomDatabase(ctkDICOMDatabasePtr database, QString folder)
{
	QSharedPointer<ctkDICOMIndexer> DICOMIndexer = QSharedPointer<ctkDICOMIndexer> (new ctkDICOMIndexer); //TODO: Reuse instead on creating new one?
//	std::cout.setstate(std::ios_base::failbit);//Hack to silence std::cout
//	DICOMIndexer->addDirectory(*database,folder,"");//This function prints out (with std::cout) a list of all files (ctkDICOMIndexer.cpp, line 93)


	connect(DICOMIndexer.get(), &ctkDICOMIndexer::indexingComplete, this, &DICOMReader::indexingCompleteSlot);

//	DICOMIndexer->addDirectory(database.data(), folder);

	//Test
	DICOMIndexer->setDatabase(database.data());

//	DICOMIndexer->setDatabase(database);
	DICOMIndexer->addDirectory(folder);
//	DICOMIndexer->addDicomdir(folder);
	CX_LOG_DEBUG() << "DICOMIndexer->addDirectory finished. isImporting:" << DICOMIndexer->isImporting();

//	std::cout.clear();//Turn on std::cout again


	CX_LOG_DEBUG() << "DICOMReader::addFolderToDicomDatabase database->seriesCount(): " << database->seriesCount();
}

//Debug function
void DICOMReader::indexingCompleteSlot(int patientsAdded, int studiesAdded, int seriesAdded, int imagesAdded)
{
	CX_LOG_DEBUG() << "indexingComplete. patientsAdded: " << patientsAdded << " studiesAdded: " << studiesAdded << " seriesAdded: " << seriesAdded << " imagesAdded: " << imagesAdded;
}

void DICOMReader::stopDCMTKMessages()
{
	//DCMTK sometimes prints a lot of warnings about zeroes in the data:
	//W: DcmItem: Element (0000,0000) found twice in one dataset/item, ignoring second entry
	//This can delay the application several minutes

	//Still this soution is also a bit slow. Removing the warning print line in DCMTK is faster:
	//in DcmItem::readSubElement() - line 1104 and 1105:
	//DCMDATA_WARN("DcmItem: Element " << newTag << " found twice in one dataset/item, ignoring second entry");

	//Example from: https://support.dcmtk.org/redmine/projects/dcmtk/wiki/howto_logprogram
	dcmtk::log4cplus::SharedAppenderPtr nullapp(new dcmtk::log4cplus::NullAppender());
	/* make sure that only the null logger is used */
	dcmtk::log4cplus::Logger log = dcmtk::log4cplus::Logger::getRoot();
	log.removeAllAppenders();
	log.addAppender(nullapp);
}

std::vector<ImagePtr> DICOMReader::importBestSeries(ctkDICOMDatabasePtr database)
{
	std::vector<ImagePtr> retval;
	QString seriesUid = this->getBestDICOMSeries(database);

	cx::DicomConverter converter;
	converter.setDicomDatabase(database.data());
	cx::ImagePtr convertedImage = converter.convertToImage(seriesUid);

	if (convertedImage)
		retval.push_back(convertedImage);
	return retval;
}

std::vector<ImagePtr> DICOMReader::importAllSeries(ctkDICOMDatabasePtr database, QProgressDialog &progress)
{
	QStringList allSeriesUid = this->getAllDICOMSeries(database);
	CX_LOG_DEBUG() << "DICOMReader::importAllSeries: " << allSeriesUid.join(", ");

	progress.setLabelText("Converting DICOM series...");
	progress.setMaximum(allSeriesUid.size());

	cx::DicomConverter converter;
	converter.setDicomDatabase(database.data());

	std::vector<ImagePtr> retval;
	for(int i = 0; i < allSeriesUid.size(); ++i)
	{
		progress.setValue(i);
		if (progress.wasCanceled())
			break;
		cx::ImagePtr convertedImage = converter.convertToImage(allSeriesUid[i]);

		if (convertedImage)
			retval.push_back(convertedImage);
		else
			reportError(QString("Failed to convert DICOM series %1").arg(allSeriesUid[i]));
	}

	return retval;
}

QStringList DICOMReader::getAllDICOMSeries(ctkDICOMDatabasePtr database)
{
	QStringList series;
	QStringList patients = database.data()->patients();
	CX_LOG_DEBUG() << "DICOMReader::getAllDICOMSeries patients: " << patients.join(", ");
	CX_LOG_DEBUG() << "DICOMReader::getAllDICOMSeries database->seriesCount(): " << database->seriesCount();
	CX_LOG_DEBUG() << "DICOMReader::getAllDICOMSeries database.data()->seriesCount(): " << database.data()->seriesCount();
	CX_LOG_DEBUG() << "DICOMReader::getAllDICOMSeries studiesCount: " << database.data()->studiesCount();
	CX_LOG_DEBUG() << "DICOMReader::getAllDICOMSeries imagesCount: " << database.data()->imagesCount();
	CX_LOG_DEBUG() << "DICOMReader::getAllDICOMSeries patientsCount: " << database.data()->patientsCount();
	for(int pNr = 0; pNr < patients.size(); ++pNr)
	{
		QString patient = patients[pNr];
		CX_LOG_DEBUG() << "Got " << patients.size() << " DICOM patients.";
		QStringList studies = database->studiesForPatient(patient);
		CX_LOG_DEBUG() << "Got " << studies.size() << " DICOM studies for patient " << patient;
		for(int sNr = 0; sNr < studies.size(); ++sNr)
		{
			QString study = studies[sNr];
			series << database->seriesForStudy(study);
		}
	}
	return series;
}

//Choose the series with the most files for now
QString DICOMReader::getBestDICOMSeries(ctkDICOMDatabasePtr database)
{
	QStringList series = getAllDICOMSeries(database);
	QString retval;
	int numFiles = 0;

	for(int seriesNr = 0; seriesNr < series.size(); ++seriesNr)
	{
		QString serie = series[seriesNr];
		QStringList files = database->filesForSeries(serie);
//		CX_LOG_DEBUG() << "Got " << files.size() << " DICOM files for series " << serie;
		if(numFiles < files.size())
		{
			numFiles = files.size();
			retval = serie;
		}
	}
	return retval;
}

}//cx
