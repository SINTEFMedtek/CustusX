/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxNIfTIReader.h"

#include <QFileInfo>
#include "vtkNIFTIImageReader.h"
#include "vtkMatrix4x4.h"
#include "vtkImageData.h"
#include "cxRegistrationTransform.h"
#include "cxTypeConversions.h"
#include "cxErrorObserver.h"
#include "cxImage.h"
#include "cxPatientModelService.h"

namespace cx
{

NIfTIReader::NIfTIReader(PatientModelServicePtr patientModelService) :
	FileReaderWriterImplService("NIfTIReader", Image::getTypeName(), "", "nii", patientModelService)
{
	sform_matrix = vtkMatrix4x4Ptr::New();
}

NIfTIReader::~NIfTIReader()
{
}


bool NIfTIReader::readInto(ImagePtr image, QString path)
{
	if (!image)
		return false;

	vtkImageDataPtr raw = this->loadVtkImageData(path);
	if(!raw)
		return false;

	Transform3D rMd(sform_matrix);
	image->setVtkImageData(raw);
	image->get_rMd_History()->setRegistration(rMd);

	return true;
}


bool NIfTIReader::readInto(DataPtr data, QString filename)
{
	return this->readInto(boost::dynamic_pointer_cast<Image>(data), filename);
}

vtkImageDataPtr NIfTIReader::loadVtkImageData(QString filename)
{
	vtkNIFTIImageReaderPtr reader = vtkNIFTIImageReaderPtr::New();
	reader->SetFileName(cstring_cast(filename));
	reader->ReleaseDataFlagOn();
	reader->Update();

	/** https://nifti.nimh.nih.gov/nifti-1/documentation/faq#Q19
	Q19. Why does NIfTI-1 allow for two coordinate systems (the qform and sform)? (Mark Jenkinson)
	The basic idea behind having two coordinate systems is to allow the image to store information about (1) the scanner coordinate system used in the acquisition of the volume (in the qform) and (2) the relationship to a standard coordinate system - e.g. MNI coordinates (in the sform).

	The qform allows orientation information to be kept for alignment purposes without losing volumetric information, since the qform only stores a rigid-body transformation which preserves volume. On the other hand, the sform stores a general affine transformation which can map the image coordinates into a standard coordinate system, like Talairach or MNI, without the need to resample the image.

	By having both coordinate systems, it is possible to keep the original data (without resampling), along with information on how it was acquired (qform) and how it relates to other images via a standard space (sform). This ability is advantageous for many analysis pipelines, and has previously required storing additional files along with the image files. By using NIfTI-1 this extra information can be kept in the image files themselves.

	Note: the qform and sform also store information on whether the coordinate system is left-handed or right-handed (see Q15) and so when both are set they must be consistent, otherwise the handedness of the coordinate system (often used to distinguish left-right order) is unknown and the results of applying operations to such an image are unspecified.

	*/
	sform_matrix = reader->GetSFormMatrix();

	if (!ErrorObserver::checkedRead(reader, filename))
		return vtkImageDataPtr();

	return reader->GetOutput();
}

bool NIfTIReader::isNull()
{
	return false;
}

QString NIfTIReader::canReadDataType() const
{
	return Image::getTypeName();
}

bool NIfTIReader::canRead(const QString &type, const QString &filename)
{
	return this->canReadInternal(type, filename);
}

DataPtr NIfTIReader::read(const QString &uid, const QString &filename)
{
	ImagePtr image(new Image(uid, vtkImageDataPtr()));
	this->readInto(image, filename);
	return image;
}

std::vector<DataPtr> NIfTIReader::read(const QString &filename)
{
	std::vector<DataPtr> retval;
	ImagePtr image = boost::dynamic_pointer_cast<Image>(this->createData(Image::getTypeName(), filename));

	vtkImageDataPtr raw = this->loadVtkImageData(filename);
	if(!raw)
		return retval;

	Transform3D rMd(sform_matrix);
	image->setVtkImageData(raw);
	image->get_rMd_History()->setRegistration(rMd);


	retval.push_back(image);
	return retval;
}

QString NIfTIReader::canWriteDataType() const
{
	return "";
}

bool NIfTIReader::canWrite(const QString &type, const QString &filename) const
{
	return this->canWriteInternal(type, filename);
}

void NIfTIReader::write(cx::DataPtr data, const QString &filename)
{
}

} //namespace cx

