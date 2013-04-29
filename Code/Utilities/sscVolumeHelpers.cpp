#include "sscVolumeHelpers.h"

#include <vtkUnsignedCharArray.h>
#include <vtkImageData.h>
#include <vtkPointData.h>
#include <vtkDoubleArray.h>

#include "sscImage.h"
#include "sscDataManagerImpl.h"

#include <vtkImageResample.h>
#include <vtkImageClip.h>
#include "vtkImageShiftScale.h"

#include "sscImage.h"
#include "sscDataManager.h"
#include "sscUtilHelpers.h"
#include "sscImageTF3D.h"
#include "sscImageLUT2D.h"
#include "sscRegistrationTransform.h"
#include "sscMessageManager.h"
#include "sscEnumConverter.h"
#include "sscTime.h"

typedef vtkSmartPointer<vtkDoubleArray> vtkDoubleArrayPtr;
typedef vtkSmartPointer<class vtkImageShiftScale> vtkImageShiftScalePtr;

namespace ssc
{

vtkImageDataPtr generateVtkImageData(Eigen::Array3i dim,
                                     Vector3D spacing,
                                     const unsigned char initValue,
                                     int components)
{
	vtkImageDataPtr data = vtkImageDataPtr::New();
	data->SetSpacing(spacing[0], spacing[1], spacing[2]);
	data->SetExtent(0, dim[0]-1, 0, dim[1]-1, 0, dim[2]-1);
	data->SetScalarTypeToUnsignedChar();
	data->SetNumberOfScalarComponents(components);
	data->AllocateScalars();

	int scalarSize = dim[0]*dim[1]*dim[2]*components;

	unsigned char* ptr = reinterpret_cast<unsigned char*>(data->GetScalarPointer());
	std::fill(ptr, ptr+scalarSize, initValue);

	// A trick to get a full LUT in ssc::Image (automatic LUT generation)
	// Can't seem to fix this by calling Image::resetTransferFunctions() after volume is modified
	ptr[0] = 255;
	data->GetScalarRange();// Update internal data in vtkImageData. Seems like it is not possible to update this data after the volume has been changed.
	ptr[0] = 0;

	data->UpdateInformation(); // update extent etc

	return data;
}

vtkImageDataPtr generateVtkImageDataDouble(Eigen::Array3i dim,
                                           ssc::Vector3D spacing,
                                           double initValue)
{
	vtkImageDataPtr data = vtkImageDataPtr::New();
	data->SetSpacing(spacing[0], spacing[1], spacing[2]);
	data->SetExtent(0, dim[0]-1, 0, dim[1]-1, 0, dim[2]-1);
	data->SetScalarTypeToDouble();
	data->SetNumberOfScalarComponents(1);

	int scalarSize = dim[0]*dim[1]*dim[2];

	double *rawchars = (double*)malloc((scalarSize+1)*8);
	std::fill(rawchars,rawchars+scalarSize, initValue);

	vtkDoubleArrayPtr array = vtkDoubleArrayPtr::New();
	array->SetNumberOfComponents(1);
	//TODO: Whithout the +1 the volume is black
	array->SetArray(rawchars, scalarSize+1, 0); // take ownership
	data->GetPointData()->SetScalars(array);

	// A trick to get a full LUT in ssc::Image (automatic LUT generation)
	// Can't seem to fix this by calling Image::resetTransferFunctions() after volume is modified
	rawchars[0] = 255;
	data->GetScalarRange();// Update internal data in vtkImageData. Seems like it is not possible to update this data after the volume has been changed.
	rawchars[0] = 0;


	return data;
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
ImagePtr convertImageToUnsigned(ImagePtr image, vtkImageDataPtr suggestedConvertedVolume, bool verbose)
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
		cast->SetInput(input);
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
			ssc::messageManager()->sendInfo(QString("Converting image %1 from %2 to %3, shift=%4")
											.arg(image->getName())
											.arg(input->GetScalarTypeAsString())
											.arg(cast->GetOutput()->GetScalarTypeAsString())
											.arg(shift));
		convertedImageData = cast->GetOutput();
	}

	ImagePtr retval = ssc::dataManager()->createDerivedImage(convertedImageData, image->getUid()+"_u", image->getName()+" u", image, "");

	ssc::ImageTF3DPtr TF3D = retval->getTransferFunctions3D()->createCopy(retval->getBaseVtkImageData());
	ssc::ImageLUT2DPtr LUT2D = retval->getLookupTable2D()->createCopy(retval->getBaseVtkImageData());
	TF3D->shift(shift);
	LUT2D->shift(shift);
	retval->resetTransferFunction(TF3D, LUT2D);

	return retval;
}

std::map<std::string, std::string> getDisplayFriendlyInfo(ssc::ImagePtr image)
{
	std::map<std::string, std::string> retval;
	if(!image)
		return retval;

	//ssc::image
	retval["Filepath"] = image->getFilePath().toStdString();
	retval["Coordinate system"] = image->getCoordinateSystem().toString().toStdString();
	retval["Image type"] = image->getImageType().toStdString();
	retval["Scalar minimum"] = string_cast(image->getMin());
	retval["Scalar maximum"] = string_cast(image->getMax());
	retval["Range (max - min)"] = string_cast(image->getRange());
	retval["Maximum alpha value"] = string_cast(image->getMaxAlphaValue());
	retval["Modality"] = image->getModality().toStdString();
	retval["Name"] = image->getName().toStdString();
	retval["Parent space"] = image->getParentSpace().toStdString();
	retval["Shading"] = image->getShadingOn() ? "on" : "off";
	retval["Space"] = image->getSpace().toStdString();
	retval["Type"] = image->getType().toStdString();
	retval["Uid"] = image->getUid().toStdString();
	retval["Acquisition time"] = string_cast(image->getAcquisitionTime().toString(ssc::timestampSecondsFormatNice()));

	//vtImageData
	double spacing_x, spacing_y, spacing_z;
	image->getBaseVtkImageData()->GetSpacing(spacing_x, spacing_y, spacing_z);
	retval["Spacing"] = string_cast(spacing_x)+" mm , "+string_cast(spacing_y)+" mm , "+string_cast(spacing_z)+" mm ";
	int dims[3];
	image->getBaseVtkImageData()->GetDimensions(dims);
	retval["Dimensions"] = string_cast(dims[0])+" , "+string_cast(dims[1])+" , "+string_cast(dims[2]);
	float actualMemorySizeKB = (float)image->getBaseVtkImageData()->GetActualMemorySize();
	retval["Actual memory size"] = string_cast(actualMemorySizeKB/(1024*1024))+" GB, "+string_cast(actualMemorySizeKB/1024)+" MB, "+string_cast(actualMemorySizeKB)+" kB";
	retval["Scalar components"] = string_cast(image->getBaseVtkImageData()->GetNumberOfScalarComponents());
	retval["rMd"] = string_cast(image->get_rMd());
	retval["Number of components for points"] = string_cast(image->getBaseVtkImageData()->GetPointData()->GetNumberOfComponents());
	retval["Scalar type"] = string_cast(image->getBaseVtkImageData()->GetScalarTypeAsString());

	return retval;
}


} // namespace ssc
