/*
 * sscVolumeHelpers.cpp
 *
 *  Created on: Dec 13, 2010
 *      Author: dev
 */

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

typedef vtkSmartPointer<vtkDoubleArray> vtkDoubleArrayPtr;
typedef vtkSmartPointer<class vtkImageShiftScale> vtkImageShiftScalePtr;

namespace ssc
{

//---------------------------------------------------------
//---------------------------------------------------------
//---------------------------------------------------------

vtkImageDataPtr generateVtkImageData(Eigen::Array3i dim,
                                     Vector3D spacing,
                                     const unsigned char initValue)
{
	vtkImageDataPtr data = vtkImageDataPtr::New();
	data->SetSpacing(spacing[0], spacing[1], spacing[2]);
	data->SetExtent(0, dim[0]-1, 0, dim[1]-1, 0, dim[2]-1);
	data->SetScalarTypeToUnsignedChar();
	data->SetNumberOfScalarComponents(1);

	int scalarSize = dim[0]*dim[1]*dim[2];

	scalarSize += 1;	//TODO: Whithout the +1 the volume is black

	unsigned char *rawchars = (unsigned char*)malloc(scalarSize);
	std::fill(rawchars,rawchars+scalarSize, initValue);

	vtkUnsignedCharArrayPtr array = vtkUnsignedCharArrayPtr::New();
	array->SetNumberOfComponents(1);
	array->SetArray(rawchars, scalarSize, 0); // take ownership
	data->GetPointData()->SetScalars(array);

	// A trick to get a full LUT in ssc::Image (automatic LUT generation)
	// Can't seem to fix this by calling Image::resetTransferFunctions() after volume is modified
	rawchars[0] = 255;
	data->GetScalarRange();// Update internal data in vtkImageData. Seems like it is not possible to update this data after the volume has been changed.
	rawchars[0] = 0;


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

//	if (input->GetScalarRange()[0] >= 0) // wrong: must convert type even if all data are positive
//		return input;
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
			ssc::messageManager()->sendInfo(QString("Converting image %1 from %2 to %3").arg(image->getName()).arg(input->GetScalarTypeAsString()).arg(cast->GetOutput()->GetScalarTypeAsString()));
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


} // namespace ssc
