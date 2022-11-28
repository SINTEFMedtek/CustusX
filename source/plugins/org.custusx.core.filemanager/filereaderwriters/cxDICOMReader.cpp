/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxDICOMReader.h"

#include <QDir>
#include <vtkImageData.h>
#include <ctkDICOMDatabase.h>
#include <ctkDICOMIndexer.h>
#include "cxImage.h"
#include "cxPatientModelService.h"
#include "cxLogger.h"
#include "cxDicomConverter.h"
#include "cxDicomImageReader.h"
#include "cxReporter.h"

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
	//Assume DICOM if filename is a directory
	if(QFileInfo(filename).isDir())
		return true;

	QString fileType = QFileInfo(filename).suffix();
	QFile file(filename);
	bool opened = file.open(QIODevice::ReadOnly);

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

//Copied from DicomWidget::importSeries
//Also copied DicomConverter and DicomImageReader files from the dicom plugin
std::vector<ImagePtr> DICOMReader::importSeries(QString fileName, bool readBestSeries)
{
	//Turn off Qt messages temporarily
	CX_LOG_DEBUG() << "stopQtMessages";
	reporter()->stopQtMessages();

	ctkDICOMDatabasePtr database = ctkDICOMDatabasePtr(new ctkDICOMDatabase);
	database->openDatabase(":memory:");

	QFileInfo dir = QFileInfo(fileName);
	QString folder = dir.absolutePath();

	QSharedPointer<ctkDICOMIndexer> DICOMIndexer = QSharedPointer<ctkDICOMIndexer> (new ctkDICOMIndexer);
	DICOMIndexer->addDirectory(*database,folder,"");//This function prints out a list of all files
	std::vector<ImagePtr> retval;
	if(readBestSeries)
		retval = importBestSeries(database);
	else
		retval = importAllSeries(database);

	database->closeDatabase();

	//Turn Qt messages back on
	reporter()->startQtMessages();
	CX_LOG_DEBUG() << "startQtMessages";
	return retval;
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

std::vector<ImagePtr> DICOMReader::importAllSeries(ctkDICOMDatabasePtr database)
{
	//TODO: Allow user to select serie

	QStringList allSeriesUid = this->getAllDICOMSeries(database);

	cx::DicomConverter converter;
	converter.setDicomDatabase(database.data());

	std::vector<ImagePtr> retval;
	for(int i = 0; i < allSeriesUid.size(); ++i)
	{
		cx::ImagePtr convertedImage = converter.convertToImage(allSeriesUid[i]);

		if (convertedImage)
		{
//			int dims[3];
//			convertedImage->getBaseVtkImageData()->GetDimensions(dims);
//			CX_LOG_DEBUG() << "Num slizes in image: " << dims[2];
//			QStringList files = database->filesForSeries(allSeriesUid[i]);
//			CX_LOG_DEBUG() << "Num files in series: " << files.size();

			retval.push_back(convertedImage);
		}
		else
			reportError(QString("Failed to convert DICOM series %1").arg(allSeriesUid[i]));
	}

	return retval;
}

QStringList DICOMReader::getAllDICOMSeries(ctkDICOMDatabasePtr database)
{
	QStringList series;
	QStringList patients = database->patients();
	for(int pNr = 0; pNr < patients.size(); ++pNr)
	{
		QString patient = patients[pNr];
//		CX_LOG_DEBUG() << "Got " << patients.size() << " DICOM patients.";
		QStringList studies = database->studiesForPatient(patient);
//		CX_LOG_DEBUG() << "Got " << studies.size() << " DICOM studies for patient " << patient;
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
