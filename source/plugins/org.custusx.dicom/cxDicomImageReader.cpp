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

#include "cxDicomImageReader.h"
#include "cxLogger.h"

namespace cx
{

DicomImageReaderPtr DicomImageReader::createFromFile(QString filename)
{
	DicomImageReaderPtr retval(new DicomImageReader);
	if (retval->loadFile(filename))
		return retval;
	else
		return DicomImageReaderPtr();
}

DicomImageReader::DicomImageReader() :
	mDataset(NULL)
{
}

bool DicomImageReader::loadFile(QString filename)
{
//	std::cout << "*** load file: " << filename << std::endl;
	mFilename = filename;
	OFCondition status = mFileFormat.loadFile(filename.toLatin1().data());
	if( !status.good() )
	{
		return false;
	}

	mDataset = mFileFormat.getDataset();
//	mDicomItem = this->wrapInCTK(mDataset);
	return true;
}

ctkDICOMItemPtr DicomImageReader::item()
{
	return this->wrapInCTK(mDataset);
}

double DicomImageReader::getDouble(const DcmTagKey& tag, const unsigned long pos, const OFBool searchIntoSub) const
{
	double retval = 0;
	OFCondition condition;
	condition = mDataset->findAndGetFloat64(tag, retval, pos, searchIntoSub);
	if (!condition.good())
	{
		QString tagName = this->wrapInCTK(mDataset)->TagDescription(tag);
		this->error(QString("Failed to get tag %1/%2").arg(tagName).arg(pos));
	}
	return retval;
}

DicomImageReader::WindowLevel DicomImageReader::getWindowLevel() const
{
	WindowLevel retval;
	retval.center = this->getDouble(DCM_WindowCenter, 0, OFTrue);
	retval.width = this->getDouble(DCM_WindowWidth, 0, OFTrue);
	return retval;
}

//bool DicomImageReader::isSingleFile() const
//{
//	int numberOfFrames = mDicomItem->GetElementAsInteger(DCM_NumberOfFrames);
//	std::cout << "numberOfFrames: " << numberOfFrames << std::endl;
//	return numberOfFrames > 1;
//}

Transform3D DicomImageReader::getImageTransformPatient() const
{
	Vector3D pos;
	Vector3D e_x;
	Vector3D e_y;

	for (int i=0; i<3; ++i)
	{
		OFCondition condition;
		e_x[i] = this->getDouble(DCM_ImageOrientationPatient, i, OFTrue);
		e_y[i] = this->getDouble(DCM_ImageOrientationPatient, i+3, OFTrue);
		pos[i] = this->getDouble(DCM_ImagePositionPatient, i, OFTrue);
	}


//	std::cout << "imagePositionPatient: " << pos << std::endl;
//	std::cout << "imageOrientationPatientX: " << e_x << std::endl;
//	std::cout << "imageOrientationPatientY: " << e_y << std::endl;

	Transform3D retval = cx::createTransformIJC(e_x, e_y, pos);
	return retval;
}

ctkDICOMItemPtr DicomImageReader::wrapInCTK(DcmItem* item) const
{
	if (!item)
		return ctkDICOMItemPtr();
	ctkDICOMItemPtr retval(new ctkDICOMItem);
	retval->InitializeFromItem(item);
	return retval;
}

void DicomImageReader::error(QString message) const
{
	reportError(QString("Dicom convert: [%1] in %2").arg(message).arg(mFilename));
}

//void DicomImageReader::localDebug(QString message) const
//{
//	if (true)
//		reportDebug(message);
//}

vtkImageDataPtr DicomImageReader::createVtkImageData()
{
	DicomImage dicomImage(mFilename.toLatin1().data()); //, CIF_MayDetachPixelData );
	const DiPixel *pixels = dicomImage.getInterData();
	if (!pixels)
	{
		this->error("Found no pixel data");
		return vtkImageDataPtr();
	}

	vtkImageDataPtr data = vtkImageDataPtr::New();

	data->SetSpacing(this->getSpacing().data());
//	this->localDebug(QString("  spacing: %1").arg(qstring_cast(this->getSpacing())));
//	std::cout << "  this->getSpacing(): " << this->getSpacing() << std::endl;

	Eigen::Array3i dim = this->getDim(dicomImage);
	data->SetExtent(0, dim[0]-1, 0, dim[1]-1, 0, dim[2]-1);

//	std::cout << "pixels->getCount(): " << pixels->getCount() << std::endl;

	int samplesPerPixel = pixels->getPlanes();
//	int samplesPerPixel = mDicomItem->GetElementAsUnsignedShort(DCM_SamplesPerPixel);
	int scalarSize = dim.prod() * samplesPerPixel;
	int pixelDepth = dicomImage.getDepth();

	switch (pixels->getRepresentation())
	{
	case EPR_Uint8:
//		std::cout << "  VTK_UNSIGNED_CHAR" << std::endl;
		data->AllocateScalars(VTK_UNSIGNED_CHAR, samplesPerPixel);
		break;
	case EPR_Uint16:
//		std::cout << "  VTK_UNSIGNED_SHORT" << std::endl;
		data->AllocateScalars(VTK_UNSIGNED_SHORT, samplesPerPixel);
		break;
	case EPR_Uint32:
		this->error("DICOM EPR_Uint32 not supported");
		return vtkImageDataPtr();
		break;
	case EPR_Sint8:
//		std::cout << "  VTK_CHAR" << std::endl;
		data->AllocateScalars(VTK_CHAR, samplesPerPixel);
		break;
	case EPR_Sint16:
//		std::cout << "  VTK_SHORT" << std::endl;
		data->AllocateScalars(VTK_SHORT, samplesPerPixel);
		break;
	case EPR_Sint32:
		this->error("DICOM EPR_Sint32 not supported");
		return vtkImageDataPtr();
		break;
	}

	int bytesPerPixel = data->GetScalarSize() * samplesPerPixel;

//	std::cout << "  dim: " << dim << std::endl;
//	std::cout << "  pixelDepth: " << pixelDepth << std::endl;
//	std::cout << "  pixels->getRepresentation(): " << pixels->getRepresentation() << std::endl;
//	std::cout << "  pixels->getCount(): " << pixels->getCount() << std::endl;
//	std::cout << "  bytesPerPixel: " << bytesPerPixel << std::endl;

	memcpy(data->GetScalarPointer(), pixels->getData(), pixels->getCount()*bytesPerPixel);
	if (pixels->getCount()!=scalarSize)
		this->error("Mismatch in pixel counts");

	return data;
}

Eigen::Array3d DicomImageReader::getSpacing() const
{
	Eigen::Array3d spacing;
	spacing[0] = this->getDouble(DCM_PixelSpacing, 0, OFTrue);
	spacing[1] = this->getDouble(DCM_PixelSpacing, 1, OFTrue);
	spacing[2] = this->getDouble(DCM_SliceThickness, 0, OFTrue);
//	std::cout << "  spacing: " << spacing << std::endl;
	return spacing;
}

Eigen::Array3i DicomImageReader::getDim(const DicomImage& dicomImage) const
{
	Eigen::Array3i dim;
	dim[0] = dicomImage.getWidth();
	dim[1] = dicomImage.getHeight();
	dim[2] = dicomImage.getFrameCount();
	return dim;
}


} // namespace cx

