/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxMetaImageReader.h"

#include <QDir>
#include "cxTypeConversions.h"
#include <vtkMetaImageReader.h>
#include <vtkMetaImageWriter.h>
#include <vtkImageChangeInformation.h>
#include <vtkImageData.h>
#include "cxErrorObserver.h"
#include "cxCustomMetaImage.h"
#include "cxImage.h"
#include "cxLogger.h"
#include "cxRegistrationTransform.h"
#include "cxCoreServices.h"
#include "cxPatientModelService.h"

namespace cx {

vtkImageDataPtr MetaImageReader::loadVtkImageData(QString filename)
{
	//load the image from file
	vtkMetaImageReaderPtr reader = vtkMetaImageReaderPtr::New();
	reader->SetFileName(cstring_cast(filename));
	reader->ReleaseDataFlagOn();

	if (!ErrorObserver::checkedRead(reader, filename))
		return vtkImageDataPtr();

	vtkImageChangeInformationPtr zeroer = vtkImageChangeInformationPtr::New();
	zeroer->SetInputConnection(reader->GetOutputPort());
	zeroer->SetOutputOrigin(0, 0, 0);
	zeroer->Update();
	return zeroer->GetOutput();
}

MetaImageReader::MetaImageReader(PatientModelServicePtr patientModelService) :
	FileReaderWriterImplService("MetaImageReader", Image::getTypeName(), Image::getTypeName(), "mhd", patientModelService)
{
}

bool MetaImageReader::isNull()
{
	return false;
}

bool MetaImageReader::canRead(const QString &type, const QString &filename)
{
	QString fileType = QFileInfo(filename).suffix();
	return (fileType.compare("mhd", Qt::CaseInsensitive) == 0 || fileType.compare("mha", Qt::CaseInsensitive) == 0);
}

QString MetaImageReader::canReadDataType() const
{
	return Image::getTypeName();
}

bool MetaImageReader::readInto(DataPtr data, QString filename)
{
	return this->readInto(boost::dynamic_pointer_cast<Image>(data), filename);
}
bool MetaImageReader::readInto(ImagePtr image, QString filename)
{
	if (!image)
		return false;

	CustomMetaImagePtr customReader = CustomMetaImage::create(filename);
	Transform3D rMd = customReader->readTransform();

	vtkImageDataPtr raw = this->loadVtkImageData(filename);
	if(!raw)
		return false;

	image->setVtkImageData(raw);
//	ImagePtr image(new Image(uid, raw));

	//  RegistrationTransform regTrans(rMd, QFileInfo(filename).lastModified(), "From MHD file");
	//  image->get_rMd_History()->addRegistration(regTrans);
	image->get_rMd_History()->setRegistration(rMd);
	image->setModality(customReader->readModality());
	image->setImageType(customReader->readImageType());

	bool ok1 = true;
	bool ok2 = true;
	double level = customReader->readKey("WindowLevel").toDouble(&ok1);
	double window = customReader->readKey("WindowWidth").toDouble(&ok2);

	if (ok1 && ok2)
	{
		image->setInitialWindowLevel(window, level);
		image->resetTransferFunctions();
	}

	return true;
}

DataPtr MetaImageReader::read(const QString& uid, const QString& filename)
{
	ImagePtr image(new Image(uid, vtkImageDataPtr()));
	this->readInto(image, filename);
	return std::move(image);
}

std::vector<DataPtr> MetaImageReader::read(const QString &filename)
{
	std::vector<DataPtr> retval;

	ImagePtr image = boost::dynamic_pointer_cast<Image>(this->createData(Image::getTypeName(), filename));

	CustomMetaImagePtr customReader = CustomMetaImage::create(filename);
	Transform3D rMd = customReader->readTransform();

	vtkImageDataPtr raw = this->loadVtkImageData(filename);
	if(!raw)
		return retval;

	image->setVtkImageData(raw);
//	ImagePtr image(new Image(uid, raw));

	//  RegistrationTransform regTrans(rMd, QFileInfo(filename).lastModified(), "From MHD file");
	//  image->get_rMd_History()->addRegistration(regTrans);
	image->get_rMd_History()->setRegistration(rMd);
	image->setModality(customReader->readModality());
	image->setImageType(customReader->readImageType());

	bool ok1 = true;
	bool ok2 = true;
	double level = customReader->readKey("WindowLevel").toDouble(&ok1);
	double window = customReader->readKey("WindowWidth").toDouble(&ok2);

	if (ok1 && ok2)
	{
		image->setInitialWindowLevel(window, level);
		image->resetTransferFunctions();
	}

	retval.push_back(image);
	return retval;

}

void MetaImageReader::write(DataPtr data, const QString& filename)
{
	ImagePtr image = boost::dynamic_pointer_cast<Image>(data);
	if(!image)
	{
		CX_LOG_ERROR() << "MetaImageReader::write: Could not cast data to image";
		return;
	}
	if(!image->getBaseVtkImageData())
	{
		CX_LOG_ERROR() << "MetaImageReader::write: cxImage has no VtkImageData";
		return;
	}
	vtkMetaImageWriterPtr writer = vtkMetaImageWriterPtr::New();
	writer->SetInputData(image->getBaseVtkImageData());
	writer->SetFileDimensionality(3);
	writer->SetFileName(cstring_cast(filename));
	QDir().mkpath(QFileInfo(filename).path());

//	std::cout << "SAVING MHD COMPRESSED " << filename << std::endl;
//	writer->SetCompression(true);
	writer->SetCompression(false);
//	writer->Update(); // caused writing of (null).0 files - not necessary
	writer->Write();

	writer = 0;

	CustomMetaImagePtr customReader = CustomMetaImage::create(filename);
	customReader->setTransform(image->get_rMd());
	customReader->setModality(image->getModality());
	customReader->setImageType(image->getImageType());
	customReader->setKey("WindowLevel", qstring_cast(image->getInitialWindowLevel()));
	customReader->setKey("WindowWidth", qstring_cast(image->getInitialWindowWidth()));
	customReader->setKey("Creator", QString("CustusX_%1").arg(CustusX_VERSION_STRING));
}

}

QString cx::MetaImageReader::canWriteDataType() const
{
	return Image::getTypeName();
}

bool cx::MetaImageReader::canWrite(const QString &type, const QString &filename) const
{
	return this->canWriteInternal(type, filename);
}
