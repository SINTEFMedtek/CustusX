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

namespace cx
{
//TODO: create suffix list, not only single value
DICOMReader::DICOMReader(PatientModelServicePtr patientModelService) :
	FileReaderWriterImplService("DICOMReader" ,Image::getTypeName(), "", "dcm *", patientModelService)
{

}

bool DICOMReader::canRead(const QString &type, const QString &filename)
{
	QString fileType = QFileInfo(filename).suffix();
	//TODO: Not only check file type, but check if file/folder is DICOM.
	return (
				fileType.compare("dcm", Qt::CaseInsensitive) == 0
				|| fileType.compare("dicom", Qt::CaseInsensitive) == 0
				|| fileType.isEmpty());
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
	CX_LOG_DEBUG() << "cxDICOMReader::read() uid:" << uid << " filename: "<< filename;
	ImagePtr image(new Image(uid, vtkImageDataPtr()));
	this->readInto(image, filename);

	//ImagePtr image = importSeries(filename);
	DataPtr retval = image;
	return retval;
}

std::vector<DataPtr> DICOMReader::read(const QString &filename)
{
	CX_LOG_DEBUG() << "cxDICOMReader::read() filename: " << filename;
	std::vector<DataPtr> retval;
	//ImagePtr image = boost::dynamic_pointer_cast<Image>(this->createData(Image::getTypeName() , filename));

	//vtkImageDataPtr raw = this->loadVtkImageData(filename);
	//if(!raw)
	//	return retval;
	//image->setVtkImageData(raw);

	ImagePtr image = importSeries(filename);

	if(image)
		retval.push_back(image);

	return retval;
}

vtkImageDataPtr DICOMReader::loadVtkImageData(QString filename)
{
	CX_LOG_DEBUG() << "cxDICOMReader::loadVtkImageData() filename: " << filename;
//	vtkPNGReaderPtr pngReader = vtkPNGReaderPtr::New();
//	pngReader->SetFileName(filename.toStdString().c_str());
//	pngReader->Update();
//	return pngReader->GetOutput();
	//return vtkImageDataPtr();
	ImagePtr image = importSeries(filename);
	return image->getBaseVtkImageData();
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
ImagePtr DICOMReader::importSeries(QString fileName)
{
	cx::ImagePtr convertedImage;
	cx::DicomConverter converter;
	ctkDICOMDatabasePtr database = ctkDICOMDatabasePtr(new ctkDICOMDatabase);
	database->openDatabase(":memory:");
	converter.setDicomDatabase(database.data());

	QFileInfo dir = QFileInfo(fileName);
	QString folder = dir.absolutePath();
	//CX_LOG_DEBUG() << "fileName: " << fileName;
	//CX_LOG_DEBUG() << "folder: " << folder;

	QSharedPointer<ctkDICOMIndexer> DICOMIndexer = QSharedPointer<ctkDICOMIndexer> (new ctkDICOMIndexer);
	DICOMIndexer->addDirectory(*database,folder,"");

	QString seriesUid = this->getBestDICOMSeries(database);
	//CX_LOG_DEBUG() << "seriesUid: " << seriesUid;
	convertedImage = converter.convertToImage(seriesUid);

	if (!convertedImage)
	{
		reportError(QString("Failed to convert DICOM series %1").arg(seriesUid));
	}
	database->closeDatabase();
	return convertedImage;
}

//Choose the series with the most files for now
QString DICOMReader::getBestDICOMSeries(ctkDICOMDatabasePtr database)
{
	QString retval;
	int numFiles = 0;
	QStringList patients = database->patients();
	for(int pNr = 0; pNr < patients.size(); ++pNr)
	{
		QString patient = patients[pNr];
		//if(patients.size() > 1)
		CX_LOG_DEBUG() << "Got " << patients.size() << " DICOM patients.";
		QStringList studies = database->studiesForPatient(patient);
		CX_LOG_DEBUG() << "Got " << studies.size() << " DICOM studies for patient " << patient;
		for(int sNr = 0; sNr < studies.size(); ++sNr)
		{
			QString study = studies[sNr];
			QStringList series = database->seriesForStudy(study);
			CX_LOG_DEBUG() << "Got " << series.size() << " DICOM series for study " << study;
			for(int seriesNr = 0; seriesNr < series.size(); ++seriesNr)
			{
				QString serie = series[seriesNr];
				QStringList files = database->filesForSeries(serie);
				CX_LOG_DEBUG() << "Got " << files.size() << " DICOM files for series " << serie;
				if(numFiles < files.size())
				{
					numFiles = files.size();
					if(retval.isEmpty())
						CX_LOG_DEBUG() << "Selecting  patient " << patient << " study " << study
													 << " series " << serie << " with " << numFiles << " files";
					else
						CX_LOG_DEBUG() << "Found longer series: Patient " << patient << " study " << study
													 << " series " << serie << " with " << numFiles << " files";
					retval = serie;
				}
			}
		}
	}
	return retval;
}

}//cx
