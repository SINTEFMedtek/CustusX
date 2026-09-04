/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#include "catch.hpp"

#include "cxLiverSegmentationWidget.h"
#include "cxLiverVisibilityWidget.h"
#include "cxLiverWidget.h"
#include "cxVisServices.h"
#include "cxtestPatientModelServiceMock.h"
#include "cxImage.h"
#include "cxMesh.h"
#include "cxRegistrationTransform.h"
#include <vtkImageData.h>
#include <vtkSmartPointer.h>

TEST_CASE("LiverPlugin: Construct widgets with null services", "[unit][plugins][org.custusx.liver]")
{
	cx::VisServicesPtr services = cx::VisServices::getNullObjects();

	cx::LiverSegmentationWidget segmentationWidget(services);
	cx::LiverVisibilityWidget visibilityWidget(services);
	cx::LiverWidget liverWidget(services);

	CHECK(true);
}

TEST_CASE("LiverPlugin: filterLabel() is non-empty for every filter", "[unit][plugins][org.custusx.liver]")
{
	using cx::LiverSegmentationWidget;
	CHECK_FALSE(LiverSegmentationWidget::filterLabel(LiverSegmentationWidget::fkLiverPancreas).isEmpty());
	CHECK_FALSE(LiverSegmentationWidget::filterLabel(LiverSegmentationWidget::fkLiverVessels).isEmpty());
	CHECK_FALSE(LiverSegmentationWidget::filterLabel(LiverSegmentationWidget::fkLiverLesions).isEmpty());
	CHECK_FALSE(LiverSegmentationWidget::filterLabel(LiverSegmentationWidget::fkLiverSegments).isEmpty());
}

TEST_CASE("LiverPlugin: iniFileNameFor() has no MR ini for Liver Vessels only", "[unit][plugins][org.custusx.liver]")
{
	using cx::LiverSegmentationWidget;

	// Every CT filter has an ini file.
	CHECK_FALSE(LiverSegmentationWidget::iniFileNameFor(LiverSegmentationWidget::fkLiverPancreas, cx::imCT).isEmpty());
	CHECK_FALSE(LiverSegmentationWidget::iniFileNameFor(LiverSegmentationWidget::fkLiverVessels, cx::imCT).isEmpty());
	CHECK_FALSE(LiverSegmentationWidget::iniFileNameFor(LiverSegmentationWidget::fkLiverLesions, cx::imCT).isEmpty());
	CHECK_FALSE(LiverSegmentationWidget::iniFileNameFor(LiverSegmentationWidget::fkLiverSegments, cx::imCT).isEmpty());

	// Vessels has no MR-capable TotalSegmentator model - every other MR filter does.
	CHECK(LiverSegmentationWidget::iniFileNameFor(LiverSegmentationWidget::fkLiverVessels, cx::imMR).isEmpty());
	CHECK_FALSE(LiverSegmentationWidget::iniFileNameFor(LiverSegmentationWidget::fkLiverPancreas, cx::imMR).isEmpty());
	CHECK_FALSE(LiverSegmentationWidget::iniFileNameFor(LiverSegmentationWidget::fkLiverLesions, cx::imMR).isEmpty());
	CHECK_FALSE(LiverSegmentationWidget::iniFileNameFor(LiverSegmentationWidget::fkLiverSegments, cx::imMR).isEmpty());

	// CT and MR ini files for the same filter must differ.
	CHECK(LiverSegmentationWidget::iniFileNameFor(LiverSegmentationWidget::fkLiverPancreas, cx::imCT)
	      != LiverSegmentationWidget::iniFileNameFor(LiverSegmentationWidget::fkLiverPancreas, cx::imMR));

	// An unhandled modality yields no ini file for any filter.
	CHECK(LiverSegmentationWidget::iniFileNameFor(LiverSegmentationWidget::fkLiverPancreas, cx::imUS).isEmpty());
}

TEST_CASE("LiverPlugin: needsPreparation() is scoped to the heaviest-smoothing filters", "[unit][plugins][org.custusx.liver]")
{
	using cx::LiverSegmentationWidget;
	CHECK(LiverSegmentationWidget::needsPreparation(LiverSegmentationWidget::fkLiverPancreas));
	CHECK(LiverSegmentationWidget::needsPreparation(LiverSegmentationWidget::fkLiverSegments));
	CHECK_FALSE(LiverSegmentationWidget::needsPreparation(LiverSegmentationWidget::fkLiverVessels));
	CHECK_FALSE(LiverSegmentationWidget::needsPreparation(LiverSegmentationWidget::fkLiverLesions));
}

namespace
{
cx::ImagePtr createMockImage(cx::PatientModelServicePtr patient, QString uid)
{
	vtkSmartPointer<vtkImageData> raw = vtkSmartPointer<vtkImageData>::New();
	raw->SetDimensions(2, 2, 2);
	raw->AllocateScalars(VTK_UNSIGNED_CHAR, 1);
	cx::ImagePtr image(new cx::Image(uid, raw));
	patient->insertData(image);
	return image;
}
}

TEST_CASE("LiverPlugin: descendsFrom() walks a multi-hop parent-frame chain", "[unit][plugins][org.custusx.liver]")
{
	cxtest::PatientModelServiceMockPtr patient(new cxtest::PatientModelServiceMock());

	// original <- resampledCopy <- mesh, mirroring
	// LiverSegmentationWidget::prepareImageForHeavyFilter()'s output.
	cx::ImagePtr original = createMockImage(patient, "original");
	cx::ImagePtr resampledCopy = createMockImage(patient, "resampledCopy");
	resampledCopy->get_rMd_History()->setParentSpace(original->getUid());

	cx::MeshPtr mesh = patient->createSpecificData<cx::Mesh>("mesh", "mesh");
	mesh->get_rMd_History()->setParentSpace(resampledCopy->getUid());
	patient->insertData(mesh);

	CHECK(cx::LiverVisibilityWidget::descendsFrom(patient, mesh->getParentSpace(), original->getUid()));
	CHECK(cx::LiverVisibilityWidget::descendsFrom(patient, mesh->getParentSpace(), resampledCopy->getUid()));
	CHECK_FALSE(cx::LiverVisibilityWidget::descendsFrom(patient, mesh->getParentSpace(), "someOtherImage"));

	// Direct parenting (no intermediate copy) is the 1-hop case.
	cx::ImagePtr directParent = createMockImage(patient, "directParent");
	cx::MeshPtr directMesh = patient->createSpecificData<cx::Mesh>("directMesh", "directMesh");
	directMesh->get_rMd_History()->setParentSpace(directParent->getUid());
	patient->insertData(directMesh);
	CHECK(cx::LiverVisibilityWidget::descendsFrom(patient, directMesh->getParentSpace(), directParent->getUid()));
}

TEST_CASE("LiverPlugin: descendsFrom() terminates on a broken/cyclic chain", "[unit][plugins][org.custusx.liver]")
{
	cxtest::PatientModelServiceMockPtr patient(new cxtest::PatientModelServiceMock());

	// A cycle (a <-> b) should not infinite-loop, and should not falsely
	// claim descent from an uid that never actually appears in the chain.
	cx::ImagePtr a = createMockImage(patient, "a");
	cx::ImagePtr b = createMockImage(patient, "b");
	a->get_rMd_History()->setParentSpace(b->getUid());
	b->get_rMd_History()->setParentSpace(a->getUid());

	CHECK(cx::LiverVisibilityWidget::descendsFrom(patient, a->getUid(), b->getUid()));
	CHECK_FALSE(cx::LiverVisibilityWidget::descendsFrom(patient, a->getUid(), "neverInChain"));

	// A dangling parent reference (points to a uid the patient model
	// doesn't have) should terminate rather than loop or crash.
	cx::ImagePtr dangling = createMockImage(patient, "dangling");
	dangling->get_rMd_History()->setParentSpace("doesNotExist");
	CHECK_FALSE(cx::LiverVisibilityWidget::descendsFrom(patient, dangling->getUid(), "anything"));
}
