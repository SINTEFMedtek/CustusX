/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "catch.hpp"

#include <vtkImageCast.h>
#include <vtkImageData.h>
#include <vtkMath.h>
#include <vtkMetaImageReader.h>
#include <vtkMetaImageWriter.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPolyDataReader.h>
#include <vtkPolyDataWriter.h>
#include <vtkSmartPointer.h>
#include <QFileInfo>
#include <QtGlobal>
#include <QStringList>
#include <algorithm>
#include <array>
#include <cmath>
#include <iostream>
#include <iterator>
#include <limits>
#include <vector>

#include "cxBinaryThinningImageFilter3DFilter.h"
#include "cxDataLocations.h"
#include "cxImage.h"
#include "cxtestVisServices.h"
#include "vesselReg/SeansVesselReg.hxx"

/** Level 2 tests for CustusX#54 (remove the ITK dependency): the centerline (thinning)
 *  filter on real airway masks from the Dryad dataset, stored with reference results in
 *  CustusXData testing/Centerline/Airways (see the README there).
 */
namespace cxtest
{

namespace
{

const QStringList airwayPatients = {"Patient_016", "Patient_011", "Patient_013"};

QString airwaysFolder(QString patient)
{
	return cx::DataLocations::getTestDataPath() + "/testing/Centerline/Airways/" + patient;
}

QString fileBaseName(QString patient)
{
	return airwaysFolder(patient) + "/pat" + patient.section('_', 1);
}

QString maskFile(QString patient)
{
	return fileBaseName(patient) + "_airways.mhd";
}

QString referenceSkeletonFile(QString patient)
{
	return fileBaseName(patient) + "_centerline.mhd";
}

QString referenceMeshFile(QString patient)
{
	return fileBaseName(patient) + "_centerline.vtk";
}

vtkImageDataPtr readVolume(QString filename)
{
	vtkSmartPointer<vtkMetaImageReader> reader = vtkSmartPointer<vtkMetaImageReader>::New();
	reader->SetFileName(filename.toUtf8().constData());
	reader->Update();
	vtkImageDataPtr retval = vtkImageDataPtr::New();
	retval->DeepCopy(reader->GetOutput());
	return retval;
}

vtkPolyDataPtr readMesh(QString filename)
{
	vtkSmartPointer<vtkPolyDataReader> reader = vtkSmartPointer<vtkPolyDataReader>::New();
	reader->SetFileName(filename.toUtf8().constData());
	reader->Update();
	vtkPolyDataPtr retval = vtkPolyDataPtr::New();
	retval->DeepCopy(reader->GetOutput());
	return retval;
}

vtkImageDataPtr runCenterlineFilter(vtkImageDataPtr mask)
{
	cxtest::TestVisServicesPtr services = cxtest::TestVisServices::create();
	cx::BinaryThinningImageFilter3DFilter filter(services);
	cx::ImagePtr image(new cx::Image("airways", mask, "airways"));
	std::vector<vtkImageDataPtr> result = filter.execute(image, false);
	vtkImageDataPtr retval;
	if (result.size() == 1)
	{
		retval = result[0];
	}
	return retval;
}

vtkPolyDataPtr centerlineMesh(vtkImageDataPtr skeleton)
{
	cx::ImagePtr skeletonImage(new cx::Image("centerline", skeleton, "centerline"));
	return cx::SeansVesselReg::extractPolyData(skeletonImage, 1, nullptr);
}

/** Skeleton voxels as (i, j, k) indices. */
std::vector<std::array<int, 3>> skeletonVoxels(vtkImageDataPtr skeleton)
{
	std::vector<std::array<int, 3>> retval;
	int* dims = skeleton->GetDimensions();
	for (int k = 0; k < dims[2]; ++k)
	{
		for (int j = 0; j < dims[1]; ++j)
		{
			for (int i = 0; i < dims[0]; ++i)
			{
				if (skeleton->GetScalarComponentAsDouble(i, j, k, 0) != 0)
				{
					retval.push_back({i, j, k});
				}
			}
		}
	}
	return retval;
}

/** A port of the current algorithm must match exactly; another algorithm can be compared by counts and distances. */
struct CenterlineComparison
{
	int voxels = 0;
	int referenceVoxels = 0;
	int mismatchingVoxels = 0;
	int endPoints = 0;
	int referenceEndPoints = 0;
	int junctionVoxels = 0;
	int referenceJunctionVoxels = 0;
	double maxDistance_mm = 0;
	double meanDistance_mm = 0;
};

void countNeighbours(vtkImageDataPtr skeleton, const std::vector<std::array<int, 3>>& voxels, int& endPoints, int& junctionVoxels)
{
	int* dims = skeleton->GetDimensions();
	endPoints = 0;
	junctionVoxels = 0;
	for (const std::array<int, 3>& v : voxels)
	{
		int neighbours = 0;
		for (int dk = -1; dk <= 1; ++dk)
		{
			for (int dj = -1; dj <= 1; ++dj)
			{
				for (int di = -1; di <= 1; ++di)
				{
					int i = v[0] + di;
					int j = v[1] + dj;
					int k = v[2] + dk;
					bool inside = i >= 0 && j >= 0 && k >= 0 && i < dims[0] && j < dims[1] && k < dims[2];
					bool self = di == 0 && dj == 0 && dk == 0;
					if (inside && !self && skeleton->GetScalarComponentAsDouble(i, j, k, 0) != 0)
					{
						++neighbours;
					}
				}
			}
		}
		if (neighbours == 1)
		{
			++endPoints;
		}
		else if (neighbours >= 3)
		{
			++junctionVoxels;
		}
	}
}

CenterlineComparison compareWithReference(vtkImageDataPtr skeleton, vtkImageDataPtr reference)
{
	CenterlineComparison retval;
	std::vector<std::array<int, 3>> voxels = skeletonVoxels(skeleton);
	std::vector<std::array<int, 3>> referenceVoxels = skeletonVoxels(reference);
	std::sort(voxels.begin(), voxels.end());
	std::sort(referenceVoxels.begin(), referenceVoxels.end());
	retval.voxels = int(voxels.size());
	retval.referenceVoxels = int(referenceVoxels.size());
	countNeighbours(skeleton, voxels, retval.endPoints, retval.junctionVoxels);
	countNeighbours(reference, referenceVoxels, retval.referenceEndPoints, retval.referenceJunctionVoxels);

	std::vector<std::array<int, 3>> onlyInOne;
	std::set_symmetric_difference(voxels.begin(), voxels.end(), referenceVoxels.begin(), referenceVoxels.end(), std::back_inserter(onlyInOne));
	retval.mismatchingVoxels = int(onlyInOne.size());

	double* spacing = reference->GetSpacing();
	double sum = 0;
	for (const std::array<int, 3>& v : voxels)
	{
		double nearest = std::numeric_limits<double>::max();
		for (const std::array<int, 3>& r : referenceVoxels)
		{
			double dx = (v[0] - r[0]) * spacing[0];
			double dy = (v[1] - r[1]) * spacing[1];
			double dz = (v[2] - r[2]) * spacing[2];
			nearest = std::min(nearest, dx * dx + dy * dy + dz * dz);
		}
		nearest = std::sqrt(nearest);
		retval.maxDistance_mm = std::max(retval.maxDistance_mm, nearest);
		sum += nearest;
	}
	if (!voxels.empty())
	{
		retval.meanDistance_mm = sum / voxels.size();
	}
	return retval;
}

void checkCenterlineMatchesReference(QString patient)
{
	INFO(patient.toStdString());
	REQUIRE(QFileInfo::exists(maskFile(patient)));
	REQUIRE(QFileInfo::exists(referenceSkeletonFile(patient)));
	REQUIRE(QFileInfo::exists(referenceMeshFile(patient)));

	vtkImageDataPtr mask = readVolume(maskFile(patient));
	vtkImageDataPtr reference = readVolume(referenceSkeletonFile(patient));
	vtkImageDataPtr skeleton = runCenterlineFilter(mask);
	REQUIRE(skeleton);
	for (int d = 0; d < 3; ++d)
	{
		REQUIRE(skeleton->GetDimensions()[d] == reference->GetDimensions()[d]);
		CHECK(skeleton->GetSpacing()[d] == Approx(reference->GetSpacing()[d]));
		CHECK(skeleton->GetOrigin()[d] == Approx(reference->GetOrigin()[d]));
	}

	CenterlineComparison comparison = compareWithReference(skeleton, reference);
	INFO("voxels: " << comparison.voxels << " (reference " << comparison.referenceVoxels << ")"
		 << ", end points: " << comparison.endPoints << " (reference " << comparison.referenceEndPoints << ")"
		 << ", junction voxels: " << comparison.junctionVoxels << " (reference " << comparison.referenceJunctionVoxels << ")"
		 << ", max/mean distance to reference: " << comparison.maxDistance_mm << "/" << comparison.meanDistance_mm << " mm");
	CHECK(comparison.voxels > 0);
	CHECK(comparison.mismatchingVoxels == 0);

	vtkPolyDataPtr mesh = centerlineMesh(skeleton);
	vtkPolyDataPtr referenceMesh = readMesh(referenceMeshFile(patient));
	REQUIRE(mesh);
	REQUIRE(mesh->GetNumberOfPoints() == referenceMesh->GetNumberOfPoints());
	double maxPointDifference = 0;
	for (vtkIdType id = 0; id < mesh->GetNumberOfPoints(); ++id)
	{
		double* p = mesh->GetPoint(id);
		double* r = referenceMesh->GetPoint(id);
		maxPointDifference = std::max(maxPointDifference, std::sqrt(vtkMath::Distance2BetweenPoints(p, r)));
	}
	CHECK(maxPointDifference < 1e-3); // the .vtk file stores about 6 significant digits
}

} // namespace

TEST_CASE("BinaryThinningImageFilter3DFilter: airway centerline of Dryad Patient_016 matches the reference", "[integration][resource][filter][centerline]")
{
	checkCenterlineMatchesReference("Patient_016");
}

TEST_CASE("BinaryThinningImageFilter3DFilter: airway centerline of Dryad Patient_011 (1.25 mm slices) matches the reference", "[integration][resource][filter][centerline]")
{
	checkCenterlineMatchesReference("Patient_011");
}

TEST_CASE("BinaryThinningImageFilter3DFilter: airway centerline of Dryad Patient_013 (fragmented mask) matches the reference", "[integration][resource][filter][centerline]")
{
	checkCenterlineMatchesReference("Patient_013");
}

/** Not a test: rewrites the reference results in CustusXData, only when CX_WRITE_CENTERLINE_REFERENCES=1. */
TEST_CASE("Airway centerline references: write", "[hide][centerline_reference]")
{
	bool writeEnabled = qgetenv("CX_WRITE_CENTERLINE_REFERENCES") == "1";
	REQUIRE(writeEnabled);
	for (QString patient : airwayPatients)
	{
		INFO(patient.toStdString());
		vtkImageDataPtr skeleton = runCenterlineFilter(readVolume(maskFile(patient)));
		REQUIRE(skeleton);

		vtkSmartPointer<vtkImageCast> cast = vtkSmartPointer<vtkImageCast>::New();
		cast->SetInputData(skeleton);
		cast->SetOutputScalarTypeToUnsignedChar();
		vtkSmartPointer<vtkMetaImageWriter> writer = vtkSmartPointer<vtkMetaImageWriter>::New();
		writer->SetInputConnection(cast->GetOutputPort());
		writer->SetFileName(referenceSkeletonFile(patient).toUtf8().constData());
		writer->SetCompression(true);
		writer->Write();

		vtkSmartPointer<vtkPolyDataWriter> meshWriter = vtkSmartPointer<vtkPolyDataWriter>::New();
		meshWriter->SetInputData(centerlineMesh(skeleton));
		meshWriter->SetFileName(referenceMeshFile(patient).toUtf8().constData());
		meshWriter->Write();

		std::vector<std::array<int, 3>> voxels = skeletonVoxels(skeleton);
		int endPoints = 0;
		int junctionVoxels = 0;
		countNeighbours(skeleton, voxels, endPoints, junctionVoxels);
		std::cout << patient.toStdString() << ": " << voxels.size() << " skeleton voxels, " << endPoints
				  << " end points, " << junctionVoxels << " junction voxels" << std::endl;
	}
}

} // namespace cxtest
