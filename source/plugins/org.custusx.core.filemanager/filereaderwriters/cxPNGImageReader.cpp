/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxPNGImageReader.h"

#include <QFileInfo>
#include <vtkPNGReader.h>
#include <vtkImageData.h>
#include <ctkPluginContext.h>
#include "cxImage.h"
#include "cxPatientModelService.h"

typedef vtkSmartPointer<class vtkPNGReader> vtkPNGReaderPtr;
namespace cx
{

PNGImageReader::PNGImageReader(PatientModelServicePtr patientModelService) :
	FileReaderWriterImplService("PNGImageReader" ,Image::getTypeName(), "", "png", patientModelService)
{

}

bool PNGImageReader::canRead(const QString &type, const QString &filename)
{
	QString fileType = QFileInfo(filename).suffix();
	return (fileType.compare("png", Qt::CaseInsensitive) == 0);
}

bool PNGImageReader::readInto(DataPtr data, QString filename)
{
	return this->readInto(boost::dynamic_pointer_cast<Image>(data), filename);
}

bool PNGImageReader::readInto(ImagePtr image, QString filename)
{
	if (!image)
		return false;
	vtkImageDataPtr raw = this->loadVtkImageData(filename);
	if(!raw)
		return false;
	image->setVtkImageData(raw);
	return true;
}

QString PNGImageReader::canReadDataType() const
{
	return Image::getTypeName();
}

DataPtr PNGImageReader::read(const QString& uid, const QString& filename)
{
	ImagePtr image(new Image(uid, vtkImageDataPtr()));
	this->readInto(image, filename);
	return image;
}

std::vector<DataPtr> PNGImageReader::read(const QString &filename)
{
	std::vector<DataPtr> retval;
	ImagePtr image = boost::dynamic_pointer_cast<Image>(this->createData(Image::getTypeName() , filename));

	vtkImageDataPtr raw = this->loadVtkImageData(filename);
	if(!raw)
		return retval;
	image->setVtkImageData(raw);

	retval.push_back(image);
	return retval;

}

vtkImageDataPtr PNGImageReader::loadVtkImageData(QString filename)
{
	vtkPNGReaderPtr pngReader = vtkPNGReaderPtr::New();
	pngReader->SetFileName(filename.toStdString().c_str());
	pngReader->Update();
	return pngReader->GetOutput();
}

}


QString cx::PNGImageReader::canWriteDataType() const
{
	return "";
}

bool cx::PNGImageReader::canWrite(const QString &type, const QString &filename) const
{
	return false;
}
