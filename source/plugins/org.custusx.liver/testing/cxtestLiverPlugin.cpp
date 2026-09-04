/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#include "catch.hpp"

#include <QSet>
#include "cxLiverSegmentationWidget.h"
#include "cxLiverVisibilityWidget.h"
#include "cxLiverWidget.h"
#include "cxVisServices.h"
#include "cxtestVisServices.h"
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

TEST_CASE("LiverPlugin: organTypesFor() maps each filter to its own output organ types", "[unit][plugins][org.custusx.liver]")
{
	using cx::LiverSegmentationWidget;

	QList<cx::ORGAN_TYPE> pancreasTypes = LiverSegmentationWidget::organTypesFor(LiverSegmentationWidget::fkLiverPancreas);
	CHECK(pancreasTypes.contains(cx::otLIVER));
	CHECK(pancreasTypes.contains(cx::otPANCREAS));
	CHECK(pancreasTypes.size() == 2);

	QList<cx::ORGAN_TYPE> vesselsTypes = LiverSegmentationWidget::organTypesFor(LiverSegmentationWidget::fkLiverVessels);
	CHECK(vesselsTypes == (QList<cx::ORGAN_TYPE>() << cx::otLIVER_VESSELS));

	QList<cx::ORGAN_TYPE> lesionsTypes = LiverSegmentationWidget::organTypesFor(LiverSegmentationWidget::fkLiverLesions);
	CHECK(lesionsTypes == (QList<cx::ORGAN_TYPE>() << cx::otLIVER_LESIONS));

	QList<cx::ORGAN_TYPE> segmentsTypes = LiverSegmentationWidget::organTypesFor(LiverSegmentationWidget::fkLiverSegments);
	CHECK(segmentsTypes.size() == 8);
	CHECK(segmentsTypes.contains(cx::otLIVER_SEGMENT_1));
	CHECK(segmentsTypes.contains(cx::otLIVER_SEGMENT_8));
	// otLIVER_SEGMENTS_COMBINED was removed (redundant with Liver+Pancreas) -
	// make sure it doesn't silently reappear here.
	CHECK_FALSE(segmentsTypes.contains(cx::otLIVER));

	// No two filters should claim the same organ type.
	QSet<cx::ORGAN_TYPE> seen;
	foreach (cx::ORGAN_TYPE type, pancreasTypes + vesselsTypes + lesionsTypes + segmentsTypes)
	{
		CHECK_FALSE(seen.contains(type));
		seen.insert(type);
	}
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

cx::MeshPtr createMockMesh(cx::PatientModelServicePtr patient, QString uid, cx::ORGAN_TYPE organType, cx::ImagePtr parent)
{
	cx::MeshPtr mesh = patient->createSpecificData<cx::Mesh>(uid, uid);
	mesh->setOrganType(organType);
	mesh->get_rMd_History()->setParentSpace(parent->getUid());
	patient->insertData(mesh);
	return mesh;
}

class TestLiverSegmentationWidget : public cx::LiverSegmentationWidget
{
public:
	explicit TestLiverSegmentationWidget(cx::VisServicesPtr services) : cx::LiverSegmentationWidget(services) {}
	void testRemovePreviousResults(const QList<PlannedRun>& runs) const { removePreviousResults(runs); }
	void testReparentMeshesFromPreparedCopy(QString originalUid, cx::ImagePtr resampled) const
	{
		reparentMeshesFromPreparedCopy(originalUid, resampled);
	}
};
}

TEST_CASE("LiverPlugin: removePreviousResults() removes only the matching filter's own prior output", "[unit][plugins][org.custusx.liver]")
{
	cxtest::TestVisServicesPtr services = cxtest::TestVisServices::create();
	cx::PatientModelServicePtr patient = services->patient();

	cx::ImagePtr sourceA = createMockImage(patient, "sourceA");
	cx::ImagePtr sourceB = createMockImage(patient, "sourceB");

	createMockMesh(patient, "oldVesselsA", cx::otLIVER_VESSELS, sourceA);
	createMockMesh(patient, "oldVesselsB", cx::otLIVER_VESSELS, sourceB);
	createMockMesh(patient, "oldLesionsA", cx::otLIVER_LESIONS, sourceA);

	TestLiverSegmentationWidget widget(services);

	cx::LiverSegmentationWidget::PlannedRun run;
	run.filter = cx::LiverSegmentationWidget::fkLiverVessels;
	run.image = sourceA;
	widget.testRemovePreviousResults(QList<cx::LiverSegmentationWidget::PlannedRun>() << run);

	CHECK_FALSE(patient->getData<cx::Mesh>("oldVesselsA")); // removed: same filter, same source image
	CHECK(patient->getData<cx::Mesh>("oldVesselsB"));       // kept: different source image
	CHECK(patient->getData<cx::Mesh>("oldLesionsA"));       // kept: different organ type/filter
}

TEST_CASE("LiverPlugin: reparentMeshesFromPreparedCopy() preserves the mesh's world position", "[unit][plugins][org.custusx.liver]")
{
	// Regression test for a reported bug: segmented structures on a large
	// volume ended up 10-20cm off, worse in-plane than along the scan axis
	// - traced to cleanupPreparedImages() overwriting a mesh's registration
	// with the *original* (pre-crop) image's, discarding the crop offset
	// that was already correctly baked into the mesh's own registration at
	// creation time.
	cxtest::TestVisServicesPtr services = cxtest::TestVisServices::create();
	cx::PatientModelServicePtr patient = services->patient();

	cx::ImagePtr original = createMockImage(patient, "original");

	// A crop with a large offset in x/y and a small one in z, mirroring a
	// CT gantry field of view being much wider than the patient while the
	// scan itself is acquired closer to the target anatomy along z.
	cx::ImagePtr preparedCopy = createMockImage(patient, "original_prepared");
	cx::Vector3D cropOffset(150, 120, 5);
	preparedCopy->get_rMd_History()->setRegistration(original->get_rMd() * cx::createTransformTranslate(cropOffset));

	// Mesh created from the prepared copy, exactly as createOutputMesh() does:
	// registration set equal to its actual (cropped) filter input.
	cx::MeshPtr mesh = createMockMesh(patient, "liverMesh", cx::otLIVER, preparedCopy);
	mesh->get_rMd_History()->setRegistration(preparedCopy->get_rMd());
	cx::Transform3D meshWorldTransformBeforeCleanup = mesh->get_rMd();

	TestLiverSegmentationWidget widget(services);
	widget.testReparentMeshesFromPreparedCopy(original->getUid(), preparedCopy);

	CHECK(mesh->getParentSpace() == original->getUid()); // bookkeeping updated
	CHECK(cx::similar(mesh->get_rMd(), meshWorldTransformBeforeCleanup)); // position unchanged
	CHECK_FALSE(cx::similar(mesh->get_rMd(), original->get_rMd())); // i.e. NOT snapped to the original's own position
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
