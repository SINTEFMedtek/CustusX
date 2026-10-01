/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "catch.hpp"

#include <algorithm>
#include <cmath>
#include <random>
#include <vtkImageData.h>
#include <vtkImageContinuousDilate3D.h>
#include "cxBinaryThresholdImageFilter.h"
#include "cxDilationFilter.h"
#include "cxConnectedThresholdImageFilter.h"
#include "cxSmoothingImageFilter.h"

namespace cxtest
{

namespace
{

vtkImageDataPtr createImage(int dim, int scalarType, double sx, double sy, double sz)
{
	vtkImageDataPtr retval = vtkImageDataPtr::New();
	retval->SetExtent(0, dim-1, 0, dim-1, 0, dim-1);
	retval->SetSpacing(sx, sy, sz);
	retval->SetOrigin(-3, 4, 7);
	retval->AllocateScalars(scalarType, 1);
	std::fill_n(static_cast<char*>(retval->GetScalarPointer()), dim*dim*dim*retval->GetScalarSize(), 0);
	return retval;
}

double valueAt(vtkImageDataPtr image, int x, int y, int z)
{
	return image->GetScalarComponentAsDouble(x, y, z, 0);
}

int countNonZero(vtkImageDataPtr image)
{
	const int* dims = image->GetDimensions();
	int retval = 0;
	for (int z = 0; z < dims[2]; ++z)
	{
		for (int y = 0; y < dims[1]; ++y)
		{
			for (int x = 0; x < dims[0]; ++x)
			{
				if (valueAt(image, x, y, z) != 0)
				{
					++retval;
				}
			}
		}
	}
	return retval;
}

void checkSameGeometry(vtkImageDataPtr a, vtkImageDataPtr b)
{
	for (int i = 0; i < 3; ++i)
	{
		CHECK(a->GetDimensions()[i] == b->GetDimensions()[i]);
		CHECK(a->GetSpacing()[i] == Approx(b->GetSpacing()[i]));
		CHECK(a->GetOrigin()[i] == Approx(b->GetOrigin()[i]));
	}
}

} // namespace

TEST_CASE("BinaryThresholdImageFilter: threshold includes both limits", "[unit][resource][filter]")
{
	vtkImageDataPtr image = createImage(11, VTK_SHORT, 0.7, 0.7, 1.25);
	for (int x = 0; x < 11; ++x)
	{
		image->SetScalarComponentFromDouble(x, 5, 5, 0, x - 5);
	}

	vtkImageDataPtr result = cx::BinaryThresholdImageFilter::threshold(image, -1.5, 2);

	CHECK(result->GetScalarType() == VTK_UNSIGNED_CHAR);
	checkSameGeometry(image, result);
	CHECK(valueAt(result, 3, 5, 5) == 0);
	CHECK(valueAt(result, 4, 5, 5) == 1);
	CHECK(valueAt(result, 7, 5, 5) == 1);
	CHECK(valueAt(result, 8, 5, 5) == 0);
	CHECK(countNonZero(result) == 11*11*11 - 11 + 4);
}

TEST_CASE("BinaryThresholdImageFilter: limits outside the scalar range match nothing", "[unit][resource][filter]")
{
	vtkImageDataPtr image = createImage(5, VTK_UNSIGNED_CHAR, 1, 1, 1);

	CHECK(countNonZero(cx::BinaryThresholdImageFilter::threshold(image, -1000, -500)) == 0);
	CHECK(countNonZero(cx::BinaryThresholdImageFilter::threshold(image, 300, 400)) == 0);
	CHECK(countNonZero(cx::BinaryThresholdImageFilter::threshold(image, -1000, 0)) == 5*5*5);
}

TEST_CASE("BinaryThresholdImageFilter: uses the first component", "[unit][resource][filter]")
{
	vtkImageDataPtr image = vtkImageDataPtr::New();
	image->SetExtent(0, 4, 0, 4, 0, 4);
	image->AllocateScalars(VTK_UNSIGNED_CHAR, 3);
	unsigned char* values = static_cast<unsigned char*>(image->GetScalarPointer());
	std::fill_n(values, 5*5*5*3, 200);
	values[0] = 10;

	vtkImageDataPtr result = cx::BinaryThresholdImageFilter::threshold(image, 0, 50);

	CHECK(result->GetNumberOfScalarComponents() == 1);
	CHECK(countNonZero(result) == 1);
	CHECK(valueAt(result, 0, 0, 0) == 1);
}

TEST_CASE("DilationFilter: a voxel dilates to a ball of the radius in voxels", "[unit][resource][filter]")
{
	vtkImageDataPtr image = createImage(11, VTK_UNSIGNED_CHAR, 1, 1, 1);
	image->SetScalarComponentFromDouble(5, 5, 5, 0, 1);

	vtkImageDataPtr result = cx::DilationFilter::dilate(image, 2.5);

	CHECK(result->GetScalarType() == VTK_UNSIGNED_CHAR);
	checkSameGeometry(image, result);
	CHECK(countNonZero(result) == 81);
	CHECK(valueAt(result, 7, 5, 5) == 1);
	CHECK(valueAt(result, 8, 5, 5) == 0);
	CHECK(valueAt(result, 6, 6, 6) == 1);
	CHECK(valueAt(result, 7, 7, 5) == 0);
}

TEST_CASE("DilationFilter: the radius in voxels follows the spacing", "[unit][resource][filter]")
{
	vtkImageDataPtr image = createImage(11, VTK_UNSIGNED_CHAR, 0.5, 1, 2);
	image->SetScalarComponentFromDouble(5, 5, 5, 0, 1);

	vtkImageDataPtr result = cx::DilationFilter::dilate(image, 2);

	CHECK(countNonZero(result) == 71);
	CHECK(valueAt(result, 9, 5, 5) == 1);
	CHECK(valueAt(result, 5, 7, 5) == 1);
	CHECK(valueAt(result, 5, 8, 5) == 0);
	CHECK(valueAt(result, 5, 5, 6) == 1);
	CHECK(valueAt(result, 5, 5, 7) == 0);
}

TEST_CASE("DilationFilter: gives the same result as a dilation with an ellipsoid kernel", "[unit][resource][filter]")
{
	std::mt19937 rng(11);
	std::uniform_real_distribution<double> uniform(0, 1);
	const double spacings[3][3] = {{1, 1, 1}, {0.7, 0.7, 1.25}, {0.5, 0.8, 2.0}};
	const double radii[4] = {0.5, 1.0, 2.0, 3.3};
	for (int s = 0; s < 3; ++s)
	{
		for (int r = 0; r < 4; ++r)
		{
			vtkImageDataPtr image = createImage(23, VTK_UNSIGNED_CHAR, spacings[s][0], spacings[s][1], spacings[s][2]);
			unsigned char* values = static_cast<unsigned char*>(image->GetScalarPointer());
			for (int i = 0; i < 23*23*23; ++i)
			{
				values[i] = uniform(rng) < 0.01 ? 1 : 0;
			}
			values[0] = 1;

			vtkSmartPointer<vtkImageContinuousDilate3D> reference = vtkSmartPointer<vtkImageContinuousDilate3D>::New();
			reference->SetInputData(image);
			int kernelSize[3];
			for (int i = 0; i < 3; ++i)
			{
				kernelSize[i] = 2*static_cast<int>(radii[r]/spacings[s][i]) + 1;
			}
			reference->SetKernelSize(kernelSize[0], kernelSize[1], kernelSize[2]);
			reference->Update();

			vtkImageDataPtr result = cx::DilationFilter::dilate(image, radii[r]);

			const unsigned char* expected = static_cast<const unsigned char*>(reference->GetOutput()->GetScalarPointer());
			const unsigned char* actual = static_cast<const unsigned char*>(result->GetScalarPointer());
			INFO("Spacing " << s << ", radius " << radii[r]);
			CHECK(std::equal(expected, expected + 23*23*23, actual));
		}
	}
}

TEST_CASE("DilationFilter: only voxels equal to 1 are dilated", "[unit][resource][filter]")
{
	vtkImageDataPtr image = createImage(11, VTK_UNSIGNED_CHAR, 1, 1, 1);
	image->SetScalarComponentFromDouble(5, 5, 5, 0, 2);

	vtkImageDataPtr result = cx::DilationFilter::dilate(image, 1);

	CHECK(countNonZero(result) == 0);
}

TEST_CASE("ConnectedThresholdImageFilter: segments the face connected region of the seed", "[unit][resource][filter]")
{
	vtkImageDataPtr image = createImage(11, VTK_SHORT, 0.7, 0.7, 1.25);
	for (int x = 1; x <= 4; ++x)
	{
		image->SetScalarComponentFromDouble(x, 2, 2, 0, 100);
	}
	image->SetScalarComponentFromDouble(5, 3, 2, 0, 100);
	image->SetScalarComponentFromDouble(8, 8, 8, 0, 100);
	image->SetScalarComponentFromDouble(2, 3, 2, 0, 300);

	vtkImageDataPtr result = cx::ConnectedThresholdImageFilter::segment(image, 50, 200, 7, Eigen::Array3i(2, 2, 2));

	REQUIRE(result);
	CHECK(result->GetScalarType() == VTK_SHORT);
	checkSameGeometry(image, result);
	CHECK(countNonZero(result) == 4);
	CHECK(valueAt(result, 1, 2, 2) == 7);
	CHECK(valueAt(result, 4, 2, 2) == 7);
	CHECK(valueAt(result, 5, 3, 2) == 0);
	CHECK(valueAt(result, 8, 8, 8) == 0);
	CHECK(valueAt(result, 2, 3, 2) == 0);
}

TEST_CASE("ConnectedThresholdImageFilter: seed outside the image gives no result", "[unit][resource][filter]")
{
	vtkImageDataPtr image = createImage(11, VTK_SHORT, 1, 1, 1);

	CHECK_FALSE(cx::ConnectedThresholdImageFilter::segment(image, 0, 1, 1, Eigen::Array3i(11, 0, 0)));
	CHECK_FALSE(cx::ConnectedThresholdImageFilter::segment(image, 0, 1, 1, Eigen::Array3i(0, -1, 0)));
}

TEST_CASE("SmoothingImageFilter: keeps a constant image and spreads an impulse by sigma in mm", "[unit][resource][filter]")
{
	vtkImageDataPtr constant = createImage(15, VTK_SHORT, 0.5, 1, 2);
	short* values = static_cast<short*>(constant->GetScalarPointer());
	std::fill_n(values, 15*15*15, 100);
	vtkImageDataPtr smoothedConstant = cx::SmoothingImageFilter::smooth(constant, 1);
	CHECK(smoothedConstant->GetScalarType() == VTK_SHORT);
	checkSameGeometry(constant, smoothedConstant);
	CHECK(std::abs(valueAt(smoothedConstant, 7, 7, 7) - 100) <= 1);
	CHECK(std::abs(valueAt(smoothedConstant, 0, 0, 0) - 100) <= 2);

	vtkImageDataPtr impulse = createImage(15, VTK_SHORT, 0.5, 1, 2);
	impulse->SetScalarComponentFromDouble(7, 7, 7, 0, 10000);
	vtkImageDataPtr smoothedImpulse = cx::SmoothingImageFilter::smooth(impulse, 1);
	CHECK(valueAt(smoothedImpulse, 7, 7, 7) < 10000);
	CHECK(valueAt(smoothedImpulse, 9, 7, 7) > valueAt(smoothedImpulse, 7, 9, 7));
	CHECK(valueAt(smoothedImpulse, 7, 9, 7) > 0);
	CHECK(valueAt(smoothedImpulse, 7, 7, 9) == 0);
}

} // namespace cxtest
