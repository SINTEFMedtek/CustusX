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
	mFilename = filename;
	OFCondition status = mFileFormat.loadFile(filename.toLatin1().data());
	if( !status.good() )
	{
		return false;
	}

	mDataset = mFileFormat.getDataset();
	mDicomItem = this->wrapInCTK(mDataset);
	return true;
}

ctkDICOMItemPtr DicomImageReader::item()
{
	return mDicomItem;
}

bool DicomImageReader::isSingleFile() const
{
	int numberOfFrames = mDicomItem->GetElementAsInteger(DCM_NumberOfFrames);
//	std::cout << "numberOfFrames: " << numberOfFrames << std::endl;
	return numberOfFrames > 1;
}

Transform3D DicomImageReader::getImageTransformPatient() const
{
	if (this->isSingleFile())
		return this->getImageTransformPatient_singlefile();
	else
		return this->getImageTransformPatient_multifile();
}

DcmItem* DicomImageReader::findAndGetSequenceItem(DcmItem* parent, DcmTagKey tagKey, int number) const
{
	if (!parent)
		return NULL;
	DcmItem* item = NULL;
	parent->findAndGetSequenceItem(tagKey, item, number);
	return item;
}

ctkDICOMItemPtr DicomImageReader::wrapInCTK(DcmItem* item) const
{
	if (!item)
		return ctkDICOMItemPtr();
	ctkDICOMItemPtr retval(new ctkDICOMItem);
	retval->InitializeFromItem(item);
	return retval;
}

Transform3D DicomImageReader::getImageTransformPatient_singlefile() const
{
	// orientation
	DcmItem* sharedFunctionalGroupsSequence = this->findAndGetSequenceItem(mDataset,
																		   DCM_SharedFunctionalGroupsSequence);
	DcmItem* planeOrientationSequence = this->findAndGetSequenceItem(sharedFunctionalGroupsSequence,
																	 DCM_PlaneOrientationSequence);

	// position
	DcmItem* perFrameFunctionalGroupsSequence = this->findAndGetSequenceItem(mDataset,
																		   DCM_PerFrameFunctionalGroupsSequence);
	DcmItem* planePositionSequence = this->findAndGetSequenceItem(perFrameFunctionalGroupsSequence,
																	 DCM_PlanePositionSequence);

	// merge into transform
	return this->getImageTransformPatient(this->wrapInCTK(planePositionSequence),
										  this->wrapInCTK(planeOrientationSequence));
}

Transform3D DicomImageReader::getImageTransformPatient_multifile() const
{
	return this->getImageTransformPatient(mDicomItem, mDicomItem);
}

Transform3D DicomImageReader::getImageTransformPatient(ctkDICOMItemPtr planePositionItem, ctkDICOMItemPtr planeOrientationItem) const
{
	if (!planePositionItem || !planeOrientationItem)
		return Transform3D::Identity();

	Vector3D imagePositionPatient;
	Vector3D imageOrientationPatientX;
	Vector3D imageOrientationPatientY;
	for(int i=0; i<3; ++i)
	{
		imagePositionPatient[i] = planePositionItem->GetElementAsDouble(DCM_ImagePositionPatient, i);
		imageOrientationPatientX[i] = planeOrientationItem->GetElementAsDouble(DCM_ImageOrientationPatient, i);
		imageOrientationPatientY[i] = planeOrientationItem->GetElementAsDouble(DCM_ImageOrientationPatient, i+3);
	}

	//	std::cout << "singlefile: " << std::endl;
	//	std::cout << "imagePositionPatient: " << imagePositionPatient << std::endl;
	//	std::cout << "imageOrientationPatientX: " << imageOrientationPatientX << std::endl;
	//	std::cout << "imageOrientationPatientY: " << imageOrientationPatientY << std::endl;

	Transform3D retval = cx::createTransformIJC(imageOrientationPatientX,
												imageOrientationPatientY,
												imagePositionPatient);
	return retval;
}

vtkImageDataPtr DicomImageReader::createVtkImageData()
{
	DicomImage dicomImage(mFilename.toLatin1().data()); //, CIF_MayDetachPixelData );
	const DiPixel *pixels = dicomImage.getInterData();

	vtkImageDataPtr data = vtkImageDataPtr::New();

	data->SetSpacing(this->getSpacing().data());

	Eigen::Array3i dim = this->getDim(dicomImage);
	data->SetExtent(0, dim[0]-1, 0, dim[1]-1, 0, dim[2]-1);

	int samplesPerPixel = mDicomItem->GetElementAsUnsignedShort(DCM_SamplesPerPixel);
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
		reportError("Dicom convert: : DICOM EPR_Uint32 not supported");
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
		reportError("Dicom convert: : DICOM EPR_Sint32 not supported");
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
		reportError("Dicom convert: : Mismatch in pixel counts");

	return data;
}

Eigen::Array3d DicomImageReader::getSpacing() const
{
	if (this->isSingleFile())
		return this->getSpacing_singlefile();
	else
		return this->getSpacing_multifile();
}

Eigen::Array3d DicomImageReader::getSpacing_multifile() const
{
	return this->getSpacing(mDicomItem);
}

Eigen::Array3d DicomImageReader::getSpacing_singlefile() const
{
	DcmItem* sharedFunctionalGroupsSequence = this->findAndGetSequenceItem(mDataset,
																		   DCM_SharedFunctionalGroupsSequence);
	DcmItem* pixelMeasuresSequence = this->findAndGetSequenceItem(sharedFunctionalGroupsSequence,
																	 DCM_PixelMeasuresSequence);
	ctkDICOMItemPtr pixelMeasuresSequenceItem = this->wrapInCTK(pixelMeasuresSequence);

	return this->getSpacing(pixelMeasuresSequenceItem);
}

Eigen::Array3d DicomImageReader::getSpacing(ctkDICOMItemPtr item) const
{
	Eigen::Array3d spacing;
	spacing[0] = item->GetElementAsDouble(DCM_PixelSpacing, 0);
	spacing[1] = item->GetElementAsDouble(DCM_PixelSpacing, 1);
	spacing[2] = item->GetElementAsDouble(DCM_SliceThickness);
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

