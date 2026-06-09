/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#include "catch.hpp"

#include "cxClipMeshFilter.h"
#include "cxMesh.h"
#include "cxImage.h"
#include "cxLogicManager.h"
#include "cxVisServices.h"
#include "cxPatientModelService.h"
#include "cxSelectDataStringProperty.h"
#include "cxtestSessionStorageTestFixture.h"

#include <vtkPoints.h>
#include <vtkCellArray.h>
#include <vtkPolyData.h>
#include <vtkImageData.h>
#include <vtkProbeFilter.h>
#include <vtkClipPolyData.h>
#include <vtkCleanPolyData.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkTransform.h>
#include <vtkPointData.h>
#include <vtkDoubleArray.h>

namespace cxtest {

namespace {

// Two triangles in the XY plane at z=5:
//   Left triangle  (all x < 50): points 0,1,2
//   Right triangle (all x > 50): points 3,4,5
vtkSmartPointer<vtkPolyData> makeTwoTriangleMesh()
{
	vtkSmartPointer<vtkPoints> pts = vtkSmartPointer<vtkPoints>::New();
	pts->InsertNextPoint(10, 10, 5);  // 0
	pts->InsertNextPoint(10, 90, 5);  // 1
	pts->InsertNextPoint(40, 50, 5);  // 2
	pts->InsertNextPoint(60, 10, 5);  // 3
	pts->InsertNextPoint(90, 50, 5);  // 4
	pts->InsertNextPoint(60, 90, 5);  // 5

	vtkIdType tri0[3] = {0, 1, 2};
	vtkIdType tri1[3] = {3, 4, 5};
	vtkSmartPointer<vtkCellArray> cells = vtkSmartPointer<vtkCellArray>::New();
	cells->InsertNextCell(3, tri0);
	cells->InsertNextCell(3, tri1);

	vtkSmartPointer<vtkPolyData> pd = vtkSmartPointer<vtkPolyData>::New();
	pd->SetPoints(pts);
	pd->SetPolys(cells);
	return pd;
}

// 100 x 100 x 10 binary image (1 mm spacing, origin at 0):
//   x < 50  → value 0  (outside mask)
//   x >= 50 → value 1  (inside mask)
vtkSmartPointer<vtkImageData> makeHalfBinaryImage()
{
	vtkSmartPointer<vtkImageData> img = vtkSmartPointer<vtkImageData>::New();
	img->SetDimensions(100, 100, 10);
	img->SetSpacing(1.0, 1.0, 1.0);
	img->SetOrigin(0.0, 0.0, 0.0);
	img->AllocateScalars(VTK_UNSIGNED_CHAR, 1);

	for (int z = 0; z < 10; ++z)
		for (int y = 0; y < 100; ++y)
			for (int x = 0; x < 100; ++x)
			{
				unsigned char* px = static_cast<unsigned char*>(img->GetScalarPointer(x, y, z));
				*px = (x >= 50) ? 1 : 0;
			}
	return img;
}

// Runs the same probe-and-clip pipeline as ClipMeshFilter::execute().
// polyData must already be in image data space (identity transform used).
vtkSmartPointer<vtkPolyData> runClipPipeline(vtkPolyData* polyData, vtkImageData* imageData, double threshold)
{
	vtkSmartPointer<vtkTransform> identity = vtkSmartPointer<vtkTransform>::New();
	vtkSmartPointer<vtkTransformPolyDataFilter> transformFilter =
		vtkSmartPointer<vtkTransformPolyDataFilter>::New();
	transformFilter->SetInputData(polyData);
	transformFilter->SetTransform(identity);
	transformFilter->Update();

	vtkSmartPointer<vtkProbeFilter> probeFilter = vtkSmartPointer<vtkProbeFilter>::New();
	probeFilter->SetInputConnection(transformFilter->GetOutputPort());
	probeFilter->SetSourceData(imageData);
	probeFilter->Update();

	vtkPointData* pd = probeFilter->GetOutput()->GetPointData();
	vtkDataArray* probedScalars = pd->GetScalars();
	vtkDataArray* validMask = pd->GetArray("vtkValidPointMask");

	if (!probedScalars)
		return nullptr;

	vtkIdType numPts = probedScalars->GetNumberOfTuples();
	vtkSmartPointer<vtkDoubleArray> maskValues = vtkSmartPointer<vtkDoubleArray>::New();
	maskValues->SetName("MaskScalars");
	maskValues->SetNumberOfComponents(1);
	maskValues->SetNumberOfTuples(numPts);
	for (vtkIdType i = 0; i < numPts; ++i)
	{
		double val = probedScalars->GetComponent(i, 0);
		if (validMask && validMask->GetComponent(i, 0) < 0.5)
			val = 0.0;
		maskValues->SetValue(i, val);
	}

	vtkSmartPointer<vtkPolyData> meshWithMask = vtkSmartPointer<vtkPolyData>::New();
	meshWithMask->ShallowCopy(polyData);
	meshWithMask->GetPointData()->AddArray(maskValues);
	meshWithMask->GetPointData()->SetActiveScalars("MaskScalars");

	vtkSmartPointer<vtkClipPolyData> clipFilter = vtkSmartPointer<vtkClipPolyData>::New();
	clipFilter->SetInputData(meshWithMask);
	clipFilter->SetValue(threshold);
	clipFilter->GenerateClippedOutputOff();
	clipFilter->Update();

	vtkSmartPointer<vtkCleanPolyData> cleanFilter = vtkSmartPointer<vtkCleanPolyData>::New();
	cleanFilter->SetInputConnection(clipFilter->GetOutputPort());
	cleanFilter->Update();

	vtkSmartPointer<vtkPolyData> result = vtkSmartPointer<vtkPolyData>::New();
	result->DeepCopy(cleanFilter->GetOutput());
	return result;
}

} // namespace


TEST_CASE("ClipMeshFilter: instantiate", "[unit][org.custusx.filter.clipmesh]")
{
	cx::LogicManager::initialize();
	cx::VisServicesPtr services = cx::VisServices::create(cx::logicManager()->getPluginContext());

	cx::ClipMeshFilter filter(services);
	CHECK(filter.getType() == "clip_mesh_filter");
	CHECK(!filter.getName().isEmpty());
	CHECK(filter.getInputTypes().size() == 2);
	CHECK(filter.getOutputTypes().size() == 1);

	cx::LogicManager::shutdown();
}


TEST_CASE("ClipMeshFilter: binary mask removes mesh in zero region", "[unit][org.custusx.filter.clipmesh]")
{
	vtkSmartPointer<vtkPolyData> mesh = makeTwoTriangleMesh();
	vtkSmartPointer<vtkImageData> img = makeHalfBinaryImage();

	vtkSmartPointer<vtkPolyData> result = runClipPipeline(mesh, img, 0.0);

	REQUIRE(result);
	// Left triangle (x < 50, value 0) is removed; right triangle (x > 50, value 1) is kept.
	CHECK(result->GetNumberOfCells() == 1);
	CHECK(result->GetNumberOfPoints() == 3);
	for (vtkIdType i = 0; i < result->GetNumberOfPoints(); ++i)
	{
		double p[3];
		result->GetPoint(i, p);
		CHECK(p[0] > 50.0);
	}
}


TEST_CASE("ClipMeshFilter: mesh fully outside image extent is completely removed", "[unit][org.custusx.filter.clipmesh]")
{
	// Place a triangle far outside the 100x100x10 image.
	vtkSmartPointer<vtkPoints> pts = vtkSmartPointer<vtkPoints>::New();
	pts->InsertNextPoint(500, 500, 500);
	pts->InsertNextPoint(600, 500, 500);
	pts->InsertNextPoint(550, 600, 500);
	vtkIdType tri[3] = {0, 1, 2};
	vtkSmartPointer<vtkCellArray> cells = vtkSmartPointer<vtkCellArray>::New();
	cells->InsertNextCell(3, tri);
	vtkSmartPointer<vtkPolyData> mesh = vtkSmartPointer<vtkPolyData>::New();
	mesh->SetPoints(pts);
	mesh->SetPolys(cells);

	vtkSmartPointer<vtkImageData> img = makeHalfBinaryImage();

	vtkSmartPointer<vtkPolyData> result = runClipPipeline(mesh, img, 0.0);

	REQUIRE(result);
	// All points outside the image get masked to 0 and are removed.
	CHECK(result->GetNumberOfPoints() == 0);
	CHECK(result->GetNumberOfCells() == 0);
}


TEST_CASE("ClipMeshFilter: mesh fully inside non-zero region is kept intact", "[unit][org.custusx.filter.clipmesh]")
{
	// A triangle fully in the x >= 50 region (value 1) at threshold 0.
	vtkSmartPointer<vtkPoints> pts = vtkSmartPointer<vtkPoints>::New();
	pts->InsertNextPoint(60, 10, 5);
	pts->InsertNextPoint(90, 50, 5);
	pts->InsertNextPoint(60, 90, 5);
	vtkIdType tri[3] = {0, 1, 2};
	vtkSmartPointer<vtkCellArray> cells = vtkSmartPointer<vtkCellArray>::New();
	cells->InsertNextCell(3, tri);
	vtkSmartPointer<vtkPolyData> mesh = vtkSmartPointer<vtkPolyData>::New();
	mesh->SetPoints(pts);
	mesh->SetPolys(cells);

	vtkSmartPointer<vtkImageData> img = makeHalfBinaryImage();

	vtkSmartPointer<vtkPolyData> result = runClipPipeline(mesh, img, 0.0);

	REQUIRE(result);
	CHECK(result->GetNumberOfPoints() == 3);
	CHECK(result->GetNumberOfCells() == 1);
}


TEST_CASE("ClipMeshFilter: full pipeline clips mesh and creates output", "[integration][org.custusx.filter.clipmesh]")
{
	cxtest::SessionStorageTestFixture fixture;
	fixture.loadSession1();
	cx::VisServicesPtr services = fixture.mServices;

	cx::MeshPtr inputMesh(new cx::Mesh("test_mesh"));
	inputMesh->setVtkPolyData(makeTwoTriangleMesh());
	services->patient()->insertData(inputMesh);

	vtkSmartPointer<vtkImageData> imgData = makeHalfBinaryImage();
	cx::ImagePtr inputImage(new cx::Image("test_image", imgData));
	services->patient()->insertData(inputImage);

	cx::ClipMeshFilter filter(services);

	// Must call both getters to populate mInputTypes and mOutputTypes before running.
	std::vector<cx::SelectDataStringPropertyBasePtr> inputs = filter.getInputTypes();
	filter.getOutputTypes();
	inputs[0]->setValue("test_mesh");
	inputs[1]->setValue("test_image");

	REQUIRE(filter.preProcess());
	REQUIRE(filter.execute());
	REQUIRE(filter.postProcess());

	cx::StringPropertySelectMeshPtr meshProp =
		boost::dynamic_pointer_cast<cx::StringPropertySelectMesh>(filter.getOutputTypes()[0]);
	REQUIRE(meshProp);
	cx::MeshPtr outputMesh = meshProp->getMesh();
	REQUIRE(outputMesh);
	vtkPolyData* result = outputMesh->getVtkPolyData();
	REQUIRE(result);

	// Left triangle (x < 50) should be removed; right triangle (x > 50) should be kept.
	CHECK(result->GetNumberOfCells() == 1);
	CHECK(result->GetNumberOfPoints() == 3);
}

} // namespace cxtest
