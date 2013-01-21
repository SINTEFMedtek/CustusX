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


typedef vtkSmartPointer<vtkDoubleArray> vtkDoubleArrayPtr;

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

} // namespace ssc
