/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxVolumeHelpers.h"

#include <vtkUnsignedCharArray.h>
#include <vtkImageData.h>
#include <vtkPointData.h>
#include <vtkDoubleArray.h>
#include <vtkImageResample.h>
#include <vtkImageClip.h>
#include <vtkImageShiftScale.h>
#include <vtkImageAccumulate.h>
#include <vtkImageLuminance.h>
#include <vtkImageExtractComponents.h>
#include <vtkImageAppendComponents.h>

#include "cxImage.h"

#include "cxUtilHelpers.h"
#include "cxImageTF3D.h"
#include "cxImageLUT2D.h"
#include "cxRegistrationTransform.h"
#include "cxLogger.h"
#include "cxEnumConverter.h"
#include "cxTime.h"
#include "cxCoordinateSystemHelpers.h"
#include "cxPatientModelService.h"

typedef vtkSmartPointer<vtkDoubleArray> vtkDoubleArrayPtr;

namespace cx
{

namespace {
template<class TYPE, int TYPE_FROM_VTK>
vtkImageDataPtr generateVtkImageDataGeneric(Eigen::Array3i dim,
									 Vector3D spacing,
									 const TYPE initValue,
									 int components)
{
	vtkImageDataPtr data = vtkImageDataPtr::New();
	data->SetSpacing(spacing[0], spacing[1], spacing[2]);
	data->SetExtent(0, dim[0]-1, 0, dim[1]-1, 0, dim[2]-1);
//	data->SetScalarType(TYPE_FROM_VTK);
//	data->SetNumberOfScalarComponents(components);
//	data->AllocateScalars();
	data->AllocateScalars(TYPE_FROM_VTK, components);

	int scalarSize = dim[0]*dim[1]*dim[2]*components;

	TYPE* ptr = reinterpret_cast<TYPE*>(data->GetScalarPointer());
	std::fill(ptr, ptr+scalarSize, initValue);

	// FIXED: replace with setDeepModified(image)
	// A trick to get a full LUT in Image (automatic LUT generation)
	// Can't seem to fix this by calling Image::resetTransferFunctions() after volume is modified
/*	if (scalarSize > 0)
	{
		ptr[0] = 150;
//		ptr[0] = 255;
		if (scalarSize > 1)
			ptr[1] = 50;
		data->GetScalarRange();// Update internal data in vtkImageData. Seems like it is not possible to update this data after the volume has been changed.
		ptr[0] = initValue;
		if (scalarSize > 1)
			ptr[1] = initValue;
	}*/
//	data->UpdateInformation(); // update extent etc
	setDeepModified(data);

	return data;
}
} // namespace

vtkImageDataPtr generateVtkImageData(Eigen::Array3i dim,
                                     Vector3D spacing,
                                     const unsigned char initValue,
                                     int components)
{
	return generateVtkImageDataGeneric<unsigned char, VTK_UNSIGNED_CHAR>(dim, spacing, initValue, components);
}

vtkImageDataPtr generateVtkImageDataUnsignedShort(Eigen::Array3i dim,
									 Vector3D spacing,
									 const unsigned short initValue,
									 int components)
{
	return generateVtkImageDataGeneric<unsigned short, VTK_UNSIGNED_SHORT>(dim, spacing, initValue, components);
}

vtkImageDataPtr generateVtkImageDataSignedShort(Eigen::Array3i dim,
									 Vector3D spacing,
									 const short initValue,
									 int components)
{
	return generateVtkImageDataGeneric<short, VTK_SHORT>(dim, spacing, initValue, components);
}

vtkImageDataPtr generateVtkImageDataDouble(Eigen::Array3i dim,
                                           Vector3D spacing,
                                           double initValue)
{
	return generateVtkImageDataGeneric<double, VTK_DOUBLE>(dim, spacing, initValue, 1);
}

void fillShortImageDataWithGradient(vtkImageDataPtr data, int maxValue)
{
	Eigen::Array3i dim(data->GetDimensions());

	unsigned short* ptr = reinterpret_cast<unsigned short*>(data->GetScalarPointer());

//	int scalarSize = dim[0]*dim[1]*dim[2]*1;
	for (int z=0; z<dim[2]; ++z)
		for (int y=0; y<dim[1]; ++y)
			for (int x=0; x<dim[0]; ++x)
			{
				int mod = maxValue;
				int val = int((double(z)/dim[2]*mod*6))%mod;// + y%255 + x/255;
				if (val < mod/3)
					val = 0;
				ptr[z*dim[0]*dim[1] + y*dim[0] + x] = val;
			}

	//	unsigned char* ptr = reinterpret_cast<unsigned char*>(imageData->GetScalarPointer());

	//	int scalarSize = dim[0]*dim[1]*dim[2]*1;
	//	for (int z=0; z<dim[2]; ++z)
	//		for (int y=0; y<dim[1]; ++y)
	//			for (int x=0; x<dim[0]; ++x)
	//			{
	//				int val = int((double(z)/dim[2]*255*6))%255;// + y%255 + x/255;
	//				val = val/3;
	//				ptr[z*dim[0]*dim[1] + y*dim[0] + x] = val;
	//			}
	setDeepModified(data);
}

/** Create a new image containing volume data from raw, but inheriting all
 *  properties from parent.
 *
 */
ImagePtr createDerivedImage(PatientModelServicePtr dataManager, QString uid, QString name, vtkImageDataPtr raw, ImagePtr parent)
{
	ImagePtr retval = dataManager->createSpecificData<Image>(uid, name);
	retval->setVtkImageData(raw);
	retval->intitializeFromParentImage(parent);
	return retval;
}

/**Convert the input image to the smallest unsigned format.
 *
 * CT images are always shifted +1024 and converted.
 * Other images are shifted so that the smallest intensity
 * is mapped to zero.
 *
 * Either VTK_UNSIGNED_SHORT or VTK_UNSIGNED_INT is used
 * as output, depending on the input range.
 *
 */
ImagePtr convertImageToUnsigned(PatientModelServicePtr  dataManager, ImagePtr image, vtkImageDataPtr suggestedConvertedVolume, bool verbose)
{
	vtkImageDataPtr input = image->getBaseVtkImageData();

	if (input->GetScalarTypeMin() >= 0)
		return image;

	// start by shifting up to zero
	int shift = -input->GetScalarRange()[0];
	// if CT: always shift by 1024 (houndsfield units definition)
	if (image->getModality().contains("CT", Qt::CaseInsensitive))
		shift = 1024;

	vtkImageDataPtr convertedImageData = suggestedConvertedVolume; // use input if given

	// convert volume
	if (!convertedImageData)
	{
		vtkImageShiftScalePtr cast = vtkImageShiftScalePtr::New();
		cast->SetInputData(input);
		cast->ClampOverflowOn();

		cast->SetShift(shift);

		// total intensity range of voxels:
		double range = input->GetScalarRange()[1] - input->GetScalarRange()[0];

		// to to fit within smallest type
		if (range <= VTK_UNSIGNED_SHORT_MAX-VTK_UNSIGNED_SHORT_MIN)
			cast->SetOutputScalarType(VTK_UNSIGNED_SHORT);
		else if (range <= VTK_UNSIGNED_INT_MAX-VTK_UNSIGNED_INT_MIN)
			cast->SetOutputScalarType(VTK_UNSIGNED_INT);
	//	else if (range <= VTK_UNSIGNED_LONG_MAX-VTK_UNSIGNED_LONG_MIN) // not supported by vtk - it seems (crash in rendering)
	//		cast->SetOutputScalarType(VTK_UNSIGNED_LONG);
		else
			cast->SetOutputScalarType(VTK_UNSIGNED_INT);

		cast->Update();
		if (verbose)
			report(QString("Converting image %1 from %2 to %3, shift=%4")
											.arg(image->getName())
											.arg(input->GetScalarTypeAsString())
											.arg(cast->GetOutput()->GetScalarTypeAsString())
											.arg(shift));
		convertedImageData = cast->GetOutput();
	}

	ImagePtr retval = createDerivedImage(dataManager,
										 image->getUid()+"_u", image->getName()+" u",
										 convertedImageData, image);

	ImageTF3DPtr TF3D = retval->getTransferFunctions3D()->createCopy();
	ImageLUT2DPtr LUT2D = retval->getLookupTable2D()->createCopy();

	TF3D->shift(shift);
	LUT2D->shift(shift);

	retval->setLookupTable2D(LUT2D);
	retval->setTransferFunctions3D(TF3D);

	return retval;
}

std::map<std::string, std::string> getDisplayFriendlyInfo(ImagePtr image)
{
	std::map<std::string, std::string> retval;
	if(!image)
		return retval;

	//image
	retval["Filename"] = image->getFilename().toStdString();
	retval["Coordinate system"] = image->getCoordinateSystem().toString().toStdString();
	retval["Image type"] = image->getImageType().toStdString();
	retval["Scalar minimum"] = string_cast(image->getMin());
	retval["Scalar maximum"] = string_cast(image->getMax());
	retval["Range (max - min)"] = string_cast(image->getRange());
	retval["Maximum alpha value"] = string_cast(image->getMaxAlphaValue());
	retval["VTK type min value"] = string_cast(image->getVTKMinValue());
	retval["VTK type max value"] = string_cast(image->getVTKMaxValue());
	retval["Modality"] = image->getModality().toStdString();
	retval["Name"] = image->getName().toStdString();
	retval["Parent space"] = image->getParentSpace().toStdString();
	retval["Shading"] = image->getShadingOn() ? "on" : "off";
	retval["Space"] = image->getSpace().toStdString();
	retval["Type"] = image->getType().toStdString();
	retval["Uid"] = image->getUid().toStdString();
	retval["Acquisition time"] = string_cast(image->getAcquisitionTime().toString(timestampSecondsFormatNice()));
	retval["Voxels with min value"] = string_cast(calculateNumVoxelsWithMinValue(image));
	retval["Voxels with max value"] = string_cast(calculateNumVoxelsWithMaxValue(image));
	retval["rMd"] = matrixAsSingleLineString(image->get_rMd());

	std::map<std::string, std::string> volumeMap = getDisplayFriendlyInfo(image->getBaseVtkImageData());
	retval.insert(volumeMap.begin(), volumeMap.end());

	return retval;
}

std::map<std::string, std::string> getDisplayFriendlyInfo(vtkImageDataPtr image)
{
	std::map<std::string, std::string> retval;
	if(!image)
		return retval;

	double spacing_x, spacing_y, spacing_z;
	image->GetSpacing(spacing_x, spacing_y, spacing_z);
	retval["Spacing"] = string_cast(spacing_x)+" mm , "+string_cast(spacing_y)+" mm , "+string_cast(spacing_z)+" mm ";
	int dims[3];
	image->GetDimensions(dims);
	retval["Dimensions"] = string_cast(dims[0])+" , "+string_cast(dims[1])+" , "+string_cast(dims[2]);
	retval["Size"] = string_cast(dims[0]*spacing_x)+" mm , "+string_cast(dims[1]*spacing_y)+" mm, "+string_cast(dims[2]*spacing_z)+" mm";
	float actualMemorySizeKB = (float)image->GetActualMemorySize();
	retval["Actual memory size"] = string_cast(actualMemorySizeKB/(1024*1024))+" GB, "+string_cast(actualMemorySizeKB/1024)+" MB, "+string_cast(actualMemorySizeKB)+" kB"+string_cast(actualMemorySizeKB*1024)+" bytes";
	retval["Scalar components"] = string_cast(image->GetNumberOfScalarComponents());
	retval["Number of components for points"] = string_cast(image->GetPointData()->GetNumberOfComponents());
	retval["Scalar type"] = string_cast(image->GetScalarTypeAsString());
	retval["Scalar size"] = string_cast(image->GetScalarSize());
	int extent[6];
	image->GetExtent(extent);
	retval["Extent"] = string_cast(extent[0])+" , "+string_cast(extent[1])+" , "+string_cast(extent[2])+" , "+string_cast(extent[3])+" , "+string_cast(extent[4])+" , "+string_cast(extent[5]);

	return retval;
}

void printDisplayFriendlyInfo(std::map<std::string, std::string> map)
{
	report("----- DisplayFriendlyInfo -----");
	std::map<std::string, std::string>::iterator it;
	for(it = map.begin(); it != map.end(); ++it)
	{
		QString message((it->first+": "+it->second).c_str());
		report(message);
	}
	report("-------------------------------");
}

int calculateNumVoxelsWithMaxValue(ImagePtr image)
{
	return static_cast<int*>(image->getHistogram()->GetOutput()->GetScalarPointer(image->getRange(), 0, 0))[0];
}
int calculateNumVoxelsWithMinValue(ImagePtr image)
{
	return static_cast<int*>(image->getHistogram()->GetOutput()->GetScalarPointer(0,0,0))[0];
}

DoubleBoundingBox3D findEnclosingBoundingBox(std::vector<DataPtr> data, Transform3D qMr)
{
	if (data.empty())
		return DoubleBoundingBox3D(0, 0, 0, 0, 0, 0);

	std::vector<Vector3D> corners_r;

	for (unsigned i = 0; i < data.size(); ++i)
	{
		Transform3D qMd = qMr * data[i]->get_rMd();
		DoubleBoundingBox3D bb = data[i]->boundingBox();

		corners_r.push_back(qMd.coord(bb.corner(0, 0, 0)));
		corners_r.push_back(qMd.coord(bb.corner(0, 0, 1)));
		corners_r.push_back(qMd.coord(bb.corner(0, 1, 0)));
		corners_r.push_back(qMd.coord(bb.corner(0, 1, 1)));
		corners_r.push_back(qMd.coord(bb.corner(1, 0, 0)));
		corners_r.push_back(qMd.coord(bb.corner(1, 0, 1)));
		corners_r.push_back(qMd.coord(bb.corner(1, 1, 0)));
		corners_r.push_back(qMd.coord(bb.corner(1, 1, 1)));
	}

	DoubleBoundingBox3D bb_sigma = DoubleBoundingBox3D::fromCloud(corners_r);
	return bb_sigma;
}

DoubleBoundingBox3D findEnclosingBoundingBox(std::vector<ImagePtr> images, Transform3D qMr)
{
	std::vector<DataPtr> datas(images.size());
	for (unsigned i = 0; i < images.size(); ++i)
		datas[i] = images[i];
	return findEnclosingBoundingBox(datas, qMr);
}

vtkImageDataPtr convertImageDataToGrayScale(vtkImageDataPtr image)
{
	vtkImageDataPtr retval = image;

	//vtkImageLuminance demands 3 components
	vtkImageDataPtr input = convertFrom4To3Components(image);

	if (input->GetNumberOfScalarComponents() > 2)
	{
		vtkSmartPointer<vtkImageLuminance> luminance = vtkSmartPointer<vtkImageLuminance>::New();
		luminance->SetInputData(input);
		luminance->Update();
		retval = luminance->GetOutput();
//		retval->Update();
	}
	return retval;
}

vtkImageDataPtr convertFrom4To3Components(vtkImageDataPtr image)
{
	vtkImageDataPtr retval = image;

	if (image->GetNumberOfScalarComponents() >= 4)
	{
		vtkImageAppendComponentsPtr merger = vtkImageAppendComponentsPtr::New();
		vtkImageExtractComponentsPtr splitterRGBA = vtkImageExtractComponentsPtr::New();
		splitterRGBA->SetInputData(image);
		splitterRGBA->SetComponents(0, 1, 2);
		merger->AddInputConnection(splitterRGBA->GetOutputPort());

		merger->Update();
		retval = merger->GetOutput();
	}
	return retval;
}

vtkImageDataPtr convertImageDataTo8Bit(vtkImageDataPtr image, double windowWidth, double windowLevel)
{
	vtkImageDataPtr retval = image;
	if (image->GetScalarSize() > 1)
		{
			vtkImageShiftScalePtr imageCast = vtkImageShiftScalePtr::New();
			imageCast->SetInputData(image);

//			double scalarMax = windowWidth/2.0 + windowLevel;
			double scalarMin = windowWidth/2.0 - windowLevel;

			double addToScalarValue = -scalarMin;
			double multiplyToScalarValue = 255/windowWidth;

			imageCast->SetShift(addToScalarValue);
			imageCast->SetScale(multiplyToScalarValue);
			imageCast->SetOutputScalarTypeToUnsignedChar();
			imageCast->ClampOverflowOn();
			imageCast->Update();
			retval = imageCast->GetOutput();
//			retval->Update();
		}
	return retval;
}

void setDeepModified(vtkImageDataPtr image)
{
	image->Modified();
	image->GetPointData()->Modified();
	image->GetPointData()->GetScalars()->Modified();
}

} // namespace cx
