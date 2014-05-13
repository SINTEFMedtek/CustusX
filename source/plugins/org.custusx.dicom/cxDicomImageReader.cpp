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

Transform3D DicomImageReader::getImageTransformPatient() const
{
	Vector3D imagePositionPatient;
	Vector3D imageOrientationPatientX;
	Vector3D imageOrientationPatientY;
	for(int i=0; i<3; ++i)
	{
		imagePositionPatient[i] = mDicomItem.GetElementAsDouble(DCM_ImagePositionPatient, i);
		imageOrientationPatientX[i] = mDicomItem.GetElementAsDouble(DCM_ImageOrientationPatient, i);
		imageOrientationPatientY[i] = mDicomItem.GetElementAsDouble(DCM_ImageOrientationPatient, i+3);
	}

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

	int samplesPerPixel = mDicomItem.GetElementAsUnsignedShort(DCM_SamplesPerPixel);
	int scalarSize = dim.prod() * samplesPerPixel;
	int pixelDepth = dicomImage.getDepth();

	switch (pixels->getRepresentation())
	{
	case EPR_Uint8:
		data->AllocateScalars(VTK_UNSIGNED_CHAR, samplesPerPixel);
		break;
	case EPR_Uint16:
		data->AllocateScalars(VTK_UNSIGNED_SHORT, samplesPerPixel);
		break;
	case EPR_Uint32:
		reportError("Dicom convert: : DICOM EPR_Uint32 not supported");
		return vtkImageDataPtr();
		break;
	case EPR_Sint8:
		data->AllocateScalars(VTK_CHAR, samplesPerPixel);
		break;
	case EPR_Sint16:
		data->AllocateScalars(VTK_SHORT, samplesPerPixel);
		break;
	case EPR_Sint32:
		reportError("Dicom convert: : DICOM EPR_Sint32 not supported");
		return vtkImageDataPtr();
		break;
	}

	int bytesPerPixel = pixelDepth/8 * samplesPerPixel;
	//		std::cout << "bytesPerPixel: " << bytesPerPixel << std::endl;
	memcpy(data->GetScalarPointer(), pixels->getData(), pixels->getCount()*bytesPerPixel);
	if (pixels->getCount()!=scalarSize)
		reportError("Dicom convert: : Mismatch in pixel counts");

	return data;
}

ctkDICOMItem& DicomImageReader::item() { return mDicomItem; }

DicomImageReader::DicomImageReader() : mDataset(NULL) {}
bool DicomImageReader::loadFile(QString filename)
{
	mFilename = filename;
	OFCondition status = mFileFormat.loadFile(filename.toLatin1().data());
	if( !status.good() )
	{
		return false;
	}

	mDataset = mFileFormat.getDataset();
	mDicomItem.InitializeFromItem(mDataset);
	return true;
}

Eigen::Array3d DicomImageReader::getSpacing() const
{
	Eigen::Array3d spacing;
	spacing[0] = mDicomItem.GetElementAsDouble(DCM_PixelSpacing, 0);
	spacing[1] = mDicomItem.GetElementAsDouble(DCM_PixelSpacing, 1);
	spacing[2] = mDicomItem.GetElementAsDouble(DCM_PixelSpacing, 2);
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

