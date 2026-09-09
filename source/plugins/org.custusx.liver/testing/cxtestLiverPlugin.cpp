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

	CHECK_FALSE(LiverSegmentationWidget::iniFileNameFor(LiverSegmentationWidget::fkLiverPancreas, cx::imCT).isEmpty());
	CHECK_FALSE(LiverSegmentationWidget::iniFileNameFor(LiverSegmentationWidget::fkLiverVessels, cx::imCT).isEmpty());
	CHECK_FALSE(LiverSegmentationWidget::iniFileNameFor(LiverSegmentationWidget::fkLiverLesions, cx::imCT).isEmpty());
	CHECK_FALSE(LiverSegmentationWidget::iniFileNameFor(LiverSegmentationWidget::fkLiverSegments, cx::imCT).isEmpty());

	CHECK(LiverSegmentationWidget::iniFileNameFor(LiverSegmentationWidget::fkLiverVessels, cx::imMR).isEmpty());
	CHECK_FALSE(LiverSegmentationWidget::iniFileNameFor(LiverSegmentationWidget::fkLiverPancreas, cx::imMR).isEmpty());
	CHECK_FALSE(LiverSegmentationWidget::iniFileNameFor(LiverSegmentationWidget::fkLiverLesions, cx::imMR).isEmpty());
	CHECK_FALSE(LiverSegmentationWidget::iniFileNameFor(LiverSegmentationWidget::fkLiverSegments, cx::imMR).isEmpty());

	CHECK(LiverSegmentationWidget::iniFileNameFor(LiverSegmentationWidget::fkLiverPancreas, cx::imCT)
	      != LiverSegmentationWidget::iniFileNameFor(LiverSegmentationWidget::fkLiverPancreas, cx::imMR));

	CHECK(LiverSegmentationWidget::iniFileNameFor(LiverSegmentationWidget::fkLiverPancreas, cx::imUS).isEmpty());
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
	CHECK_FALSE(segmentsTypes.contains(cx::otLIVER));

	QSet<cx::ORGAN_TYPE> seen;
	foreach (cx::ORGAN_TYPE type, pancreasTypes + vesselsTypes + lesionsTypes + segmentsTypes)
	{
		CHECK_FALSE(seen.contains(type));
		seen.insert(type);
	}
}

namespace
{
cx::ImagePtr createMockImage(cx::PatientModelServicePtr patient, QString uid, cx::IMAGE_MODALITY modality = cx::imCT)
{
	vtkSmartPointer<vtkImageData> raw = vtkSmartPointer<vtkImageData>::New();
	raw->SetDimensions(2, 2, 2);
	raw->AllocateScalars(VTK_UNSIGNED_CHAR, 1);
	cx::ImagePtr image(new cx::Image(uid, raw, uid, modality));
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
	cx::ImagePtr testPrepareImageForHeavyFilter(cx::ImagePtr image) const
	{
		return prepareImageForHeavyFilter(image);
	}
};

class TestLiverVisibilityWidget : public cx::LiverVisibilityWidget
{
public:
	explicit TestLiverVisibilityWidget(cx::VisServicesPtr services) : cx::LiverVisibilityWidget(services) {}
	cx::MeshPtr testFindMeshForSourceImage(cx::ORGAN_TYPE organType, cx::ImagePtr sourceImage) const
	{
		return findMeshForSourceImage(organType, sourceImage);
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

	CHECK_FALSE(patient->getData<cx::Mesh>("oldVesselsA"));
	CHECK(patient->getData<cx::Mesh>("oldVesselsB"));
	CHECK(patient->getData<cx::Mesh>("oldLesionsA"));
}

TEST_CASE("LiverPlugin: reparentMeshesFromPreparedCopy() preserves the mesh's world position", "[unit][plugins][org.custusx.liver]")
{
	cxtest::TestVisServicesPtr services = cxtest::TestVisServices::create();
	cx::PatientModelServicePtr patient = services->patient();

	cx::ImagePtr original = createMockImage(patient, "original");

	cx::ImagePtr preparedCopy = createMockImage(patient, "original_prepared");
	cx::Vector3D cropOffset(150, 120, 5);
	preparedCopy->get_rMd_History()->setRegistration(original->get_rMd() * cx::createTransformTranslate(cropOffset));

	cx::MeshPtr mesh = createMockMesh(patient, "liverMesh", cx::otLIVER, preparedCopy);
	mesh->get_rMd_History()->setRegistration(preparedCopy->get_rMd());
	cx::Transform3D meshWorldTransformBeforeCleanup = mesh->get_rMd();

	TestLiverSegmentationWidget widget(services);
	widget.testReparentMeshesFromPreparedCopy(original->getUid(), preparedCopy);

	CHECK(mesh->getParentSpace() == original->getUid());
	CHECK(cx::similar(mesh->get_rMd(), meshWorldTransformBeforeCleanup));
	CHECK_FALSE(cx::similar(mesh->get_rMd(), original->get_rMd()));
}

TEST_CASE("LiverPlugin: prepareImageForHeavyFilter() is a no-op for an already-small image", "[unit][plugins][org.custusx.liver]")
{
	cxtest::TestVisServicesPtr services = cxtest::TestVisServices::create();
	cx::PatientModelServicePtr patient = services->patient();

	cx::ImagePtr small = createMockImage(patient, "small");

	TestLiverSegmentationWidget widget(services);
	cx::ImagePtr prepared = widget.testPrepareImageForHeavyFilter(small);

	CHECK(prepared == small);
}

TEST_CASE("LiverPlugin: selectedImagesDeduplicated() keeps both images when they differ", "[unit][plugins][org.custusx.liver]")
{
	cxtest::TestVisServicesPtr services = cxtest::TestVisServices::create();
	cx::PatientModelServicePtr patient = services->patient();

	cx::ImagePtr imageA = createMockImage(patient, "imageA");
	cx::ImagePtr imageB = createMockImage(patient, "imageB");

	QList<cx::ImagePtr> images = cx::LiverSegmentationWidget::selectedImagesDeduplicated(imageA, imageB);

	REQUIRE(images.size() == 2);
	CHECK(images[0]->getUid() == "imageA");
	CHECK(images[1]->getUid() == "imageB");
}

TEST_CASE("LiverPlugin: selectedImagesDeduplicated() skips Volume 2 when it is the same image as Volume 1", "[unit][plugins][org.custusx.liver]")
{
	// Regression test: picking the same volume in both selectors must not
	// queue - and actually run - every checked filter twice against it.
	cxtest::TestVisServicesPtr services = cxtest::TestVisServices::create();
	cx::PatientModelServicePtr patient = services->patient();

	cx::ImagePtr imageA = createMockImage(patient, "imageA");

	QList<cx::ImagePtr> images = cx::LiverSegmentationWidget::selectedImagesDeduplicated(imageA, imageA);

	REQUIRE(images.size() == 1);
	CHECK(images[0]->getUid() == "imageA");
}

TEST_CASE("LiverPlugin: selectedImagesDeduplicated() handles a missing Volume 2", "[unit][plugins][org.custusx.liver]")
{
	cxtest::TestVisServicesPtr services = cxtest::TestVisServices::create();
	cx::PatientModelServicePtr patient = services->patient();

	cx::ImagePtr imageA = createMockImage(patient, "imageA");

	QList<cx::ImagePtr> images = cx::LiverSegmentationWidget::selectedImagesDeduplicated(imageA, cx::ImagePtr());

	REQUIRE(images.size() == 1);
	CHECK(images[0]->getUid() == "imageA");
}

TEST_CASE("LiverPlugin: findMeshForSourceImage() matches only the exact source image", "[unit][plugins][org.custusx.liver]")
{
	cxtest::TestVisServicesPtr services = cxtest::TestVisServices::create();
	cx::PatientModelServicePtr patient = services->patient();

	cx::ImagePtr fullVolume = createMockImage(patient, "fullVolume");
	cx::ImagePtr manualCrop = createMockImage(patient, "manualCrop");
	manualCrop->get_rMd_History()->setParentSpace(fullVolume->getUid());

	cx::MeshPtr cropMesh = createMockMesh(patient, "cropMesh", cx::otLIVER, manualCrop);
	cx::MeshPtr fullVolumeMesh = createMockMesh(patient, "fullVolumeMesh", cx::otLIVER, fullVolume);

	TestLiverVisibilityWidget widget(services);

	CHECK(widget.testFindMeshForSourceImage(cx::otLIVER, fullVolume)->getUid() == fullVolumeMesh->getUid());
	CHECK(widget.testFindMeshForSourceImage(cx::otLIVER, manualCrop)->getUid() == cropMesh->getUid());
}

TEST_CASE("LiverPlugin: findMeshForSourceImage() does not match a mesh belonging to a different (e.g. cropped) volume", "[unit][plugins][org.custusx.liver]")
{
	// Regression test: a mesh segmented from a manual crop of a volume must
	// not be shown when the crop's own source volume is selected instead -
	// they are two different, independently selectable volumes, even though
	// one is a crop of the other.
	cxtest::TestVisServicesPtr services = cxtest::TestVisServices::create();
	cx::PatientModelServicePtr patient = services->patient();

	cx::ImagePtr fullVolume = createMockImage(patient, "fullVolume2");
	cx::ImagePtr manualCrop = createMockImage(patient, "manualCrop2");
	manualCrop->get_rMd_History()->setParentSpace(fullVolume->getUid());

	createMockMesh(patient, "cropMesh2", cx::otLIVER, manualCrop);

	TestLiverVisibilityWidget widget(services);

	CHECK_FALSE(widget.testFindMeshForSourceImage(cx::otLIVER, fullVolume));
}
