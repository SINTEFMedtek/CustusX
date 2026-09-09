/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "catch.hpp"
#include "cxImageAlgorithms.h"

#include <vtkImageData.h>
#include <vtkSmartPointer.h>
#include "cxImage.h"
#include "cxImageTF3D.h"
#include "cxImageLUT2D.h"
#include "cxDataLocations.h"
#include "cxtestVisServices.h"
#include "cxtestPatientModelServiceMock.h"
#include "cxFileManagerServiceProxy.h"
#include "cxLogicManager.h"

using namespace cx;

TEST_CASE("ImageAlgorithms: resample() works", "[integration][resource][core]")
{
	cx::LogicManager::initialize();
	cx::FileManagerServicePtr filemanager = cx::FileManagerServiceProxy::create(cx::logicManager()->getPluginContext());

	cxtest::TestVisServicesPtr services = cxtest::TestVisServices::create();
	cx::PatientModelServicePtr pasm = services->patient();

	QString fname0 = cx::DataLocations::getTestDataPath() + "/testing/ResampleTest.cx3/Images/mra.mhd";
	QString fname1 = cx::DataLocations::getTestDataPath() + "/testing/ResampleTest.cx3/Images/US_01_20110222T110117_1.mhd";

	boost::dynamic_pointer_cast<cxtest::PatientModelServiceMock>(pasm)->importDataMock(fname0, fname0, filemanager);
	boost::dynamic_pointer_cast<cxtest::PatientModelServiceMock>(pasm)->importDataMock(fname1, fname1, filemanager);
	cx::ImagePtr image = pasm->getData<cx::Image>(fname0);
	cx::ImagePtr referenceImage = pasm->getData<cx::Image>(fname1);
	//	std::cout << "referenceImage base: " << referenceImage->getBaseVtkImageData() << std::endl;
	REQUIRE(image);
	REQUIRE(referenceImage);

	cx::Transform3D refMi = referenceImage->get_rMd().inv() * image->get_rMd();

	cx::ImagePtr oriented = resampleImage(pasm, image, refMi);
	REQUIRE(oriented);
	int inMin = image->getBaseVtkImageData()->GetScalarRange()[0];
	int inMax = image->getBaseVtkImageData()->GetScalarRange()[1];
	int outMin = oriented->getBaseVtkImageData()->GetScalarRange()[0];
	int outMax = oriented->getBaseVtkImageData()->GetScalarRange()[1];
	CHECK(inMin == outMin);
	//    std::cout << "inMax: " << inMax << " outMax: " << outMax << std::endl;
	CHECK(inMax == outMax);
	CHECK(image->getBaseVtkImageData() != oriented->getBaseVtkImageData());
//	CHECK(image->getTransferFunctions3D()->getVtkImageData() == image->getBaseVtkImageData());
	//  std::cout << "image:    " << image->getBaseVtkImageData() << " oriented:    " << oriented->getBaseVtkImageData() << std::endl;
	//  std::cout << "image tf: " << image->getTransferFunctions3D()->getVtkImageData() << " oriented tf: " << oriented->getTransferFunctions3D()->getVtkImageData() << std::endl;
	//Make sure the image and tf points to the same vtkImageData
//	CHECK(oriented->getTransferFunctions3D()->getVtkImageData() == oriented->getBaseVtkImageData());
//	CHECK(oriented->getLookupTable2D()->getVtkImageData() == oriented->getBaseVtkImageData());

	cx::Transform3D orient_M_ref = oriented->get_rMd().inv() * referenceImage->get_rMd();
	cx::DoubleBoundingBox3D bb_crop = cx::transform(orient_M_ref, referenceImage->boundingBox());

	// increase bb size by margin
	double margin = 5.0;
	bb_crop[0] -= margin;
	bb_crop[1] += margin;
	bb_crop[2] -= margin;
	bb_crop[3] += margin;
	bb_crop[4] -= margin;
	bb_crop[5] += margin;

	oriented->setCroppingBox(bb_crop);

	cx::ImagePtr cropped = cropImage(pasm, oriented);
	REQUIRE(cropped);
	int cropMin = cropped->getBaseVtkImageData()->GetScalarRange()[0];
	int cropMax = cropped->getBaseVtkImageData()->GetScalarRange()[1];
	CHECK(cropMin == inMin);
	CHECK(cropMax >  inMin);
	CHECK(cropMax <= inMax);
	CHECK(oriented->getBaseVtkImageData() != cropped->getBaseVtkImageData());
//	CHECK(cropped->getTransferFunctions3D()->getVtkImageData() == cropped->getBaseVtkImageData());
//	CHECK(cropped->getLookupTable2D()->getVtkImageData() == cropped->getBaseVtkImageData());

	QString uid = image->getUid() + "_resample%1";
	QString name = image->getName() + " resample%1";

	cx::ImagePtr resampled = cx::resampleImage(pasm, cropped, cx::Vector3D(referenceImage->getBaseVtkImageData()->GetSpacing()), uid, name);
	REQUIRE(resampled);
	outMin = resampled->getBaseVtkImageData()->GetScalarRange()[0];
	outMax = resampled->getBaseVtkImageData()->GetScalarRange()[1];
	//  std::cout << "outMin: " << outMin << " cropMin: " << cropMin << std::endl;
	//  CHECK(outMin == cropMin); //What happens when min == 1 instead of 0? //Looks ok in ImageTFData::fixTransferFunctions()
	CHECK(outMin >=  cropMin);
	CHECK(outMax >  cropMin);
	CHECK(outMax <= cropMax);
	CHECK(outMax <= cropMax);
	CHECK(cropped->getBaseVtkImageData() != resampled->getBaseVtkImageData());
//	CHECK(resampled->getTransferFunctions3D()->getVtkImageData() == resampled->getBaseVtkImageData());
//	CHECK(resampled->getLookupTable2D()->getVtkImageData() == resampled->getBaseVtkImageData());

	cx::LogicManager::shutdown();

}

namespace
{
/** A synthetic 20x20x20, 1mm-spacing volume: background everywhere at
 *  backgroundValue, with a 10x10x10 "body" block (indices 5..14 on every
 *  axis) at foregroundValue - mimicking a patient body surrounded by air.
 */
cx::ImagePtr createSyntheticBodyInAirImage(short backgroundValue, short foregroundValue)
{
	vtkSmartPointer<vtkImageData> raw = vtkSmartPointer<vtkImageData>::New();
	raw->SetDimensions(20, 20, 20);
	raw->SetSpacing(1.0, 1.0, 1.0);
	raw->AllocateScalars(VTK_SHORT, 1);

	short* ptr = static_cast<short*>(raw->GetScalarPointer());
	vtkIdType idx = 0;
	for (int z = 0; z < 20; ++z)
		for (int y = 0; y < 20; ++y)
			for (int x = 0; x < 20; ++x, ++idx)
			{
				bool inBody = (x >= 5 && x <= 14) && (y >= 5 && y <= 14) && (z >= 5 && z <= 14);
				ptr[idx] = inBody ? foregroundValue : backgroundValue;
			}

	return cx::ImagePtr(new cx::Image("synthetic_body_in_air", raw));
}
}

TEST_CASE("ImageAlgorithms: computeOtsuThreshold() separates two intensity populations", "[unit][resource][core]")
{
	cx::ImagePtr image = createSyntheticBodyInAirImage(-1000, 0);
	double threshold = cx::computeOtsuThreshold(image->getBaseVtkImageData());
	CHECK(threshold > -1000);
	CHECK(threshold < 0);

	cx::ImagePtr shiftedImage = createSyntheticBodyInAirImage(24, 1024);
	double shiftedThreshold = cx::computeOtsuThreshold(shiftedImage->getBaseVtkImageData());
	CHECK(shiftedThreshold > 24);
	CHECK(shiftedThreshold < 1024);
}

TEST_CASE("ImageAlgorithms: computeAutoCropBox() tightly bounds the foreground block", "[unit][resource][core]")
{
	cx::ImagePtr image = createSyntheticBodyInAirImage(-1000, 0);

	cx::DoubleBoundingBox3D box = cx::computeAutoCropBox(image, 0);
	CHECK(box[0] == Approx(5.0));
	CHECK(box[1] == Approx(14.0));
	CHECK(box[2] == Approx(5.0));
	CHECK(box[3] == Approx(14.0));
	CHECK(box[4] == Approx(5.0));
	CHECK(box[5] == Approx(14.0));

	cx::DoubleBoundingBox3D paddedBox = cx::computeAutoCropBox(image, 2);
	CHECK(paddedBox[0] == Approx(3.0));
	CHECK(paddedBox[1] == Approx(16.0));

	cx::DoubleBoundingBox3D hugePaddingBox = cx::computeAutoCropBox(image, 1000);
	CHECK(hugePaddingBox[0] == Approx(0.0));
	CHECK(hugePaddingBox[1] == Approx(19.0));
}

namespace
{
cx::ImagePtr createSyntheticImage(cx::PatientModelServicePtr pasm, QString uid, int dimX, int dimY, int dimZ, double spacing)
{
	vtkSmartPointer<vtkImageData> raw = vtkSmartPointer<vtkImageData>::New();
	raw->SetDimensions(dimX, dimY, dimZ);
	raw->SetSpacing(spacing, spacing, spacing);
	raw->AllocateScalars(VTK_UNSIGNED_CHAR, 1);
	memset(raw->GetScalarPointer(), 1, dimX * dimY * dimZ);

	cx::ImagePtr image(new cx::Image(uid, raw));
	pasm->insertData(image);
	return image;
}
}

TEST_CASE("ImageAlgorithms: resampleImageToMaxVoxelCount() shrinks a large volume", "[unit][resource][core]")
{
	cxtest::TestVisServicesPtr services = cxtest::TestVisServices::create();
	cx::PatientModelServicePtr pasm = services->patient();

	cx::ImagePtr image = createSyntheticImage(pasm, "big", 40, 40, 40, 1.0);

	cx::ImagePtr resampled = cx::resampleImageToMaxVoxelCount(pasm, image, 8000);
	REQUIRE(resampled);
	CHECK(resampled != image);

	int dims[3];
	resampled->getBaseVtkImageData()->GetDimensions(dims);
	double voxelCount = static_cast<double>(dims[0]) * dims[1] * dims[2];
	CHECK(voxelCount <= 8000 * 1.5);
}

TEST_CASE("ImageAlgorithms: resampleImageToMaxVoxelCount() is a no-op under the target", "[unit][resource][core]")
{
	cxtest::TestVisServicesPtr services = cxtest::TestVisServices::create();
	cx::PatientModelServicePtr pasm = services->patient();

	cx::ImagePtr image = createSyntheticImage(pasm, "small", 10, 10, 10, 1.0);

	cx::ImagePtr resampled = cx::resampleImageToMaxVoxelCount(pasm, image, 8000);
	CHECK(resampled == image);
}

TEST_CASE("ImageAlgorithms: resampleImageToMaxInPlaneResolution() caps x/y and keeps z spacing", "[unit][resource][core]")
{
	cxtest::TestVisServicesPtr services = cxtest::TestVisServices::create();
	cx::PatientModelServicePtr pasm = services->patient();

	cx::ImagePtr image = createSyntheticImage(pasm, "wide", 40, 40, 10, 1.0);

	cx::ImagePtr resampled = cx::resampleImageToMaxInPlaneResolution(pasm, image, 20);
	REQUIRE(resampled);
	double* spacing = resampled->getBaseVtkImageData()->GetSpacing();
	CHECK(spacing[0] == Approx(2.0));
	CHECK(spacing[1] == Approx(2.0));
	CHECK(spacing[2] == Approx(1.0));
}

TEST_CASE("ImageAlgorithms: cropImage() with an explicit box does not touch image->getCroppingBox()", "[unit][resource][core]")
{
	cxtest::TestVisServicesPtr services = cxtest::TestVisServices::create();
	cx::PatientModelServicePtr pasm = services->patient();

	cx::ImagePtr image = createSyntheticImage(pasm, "crop_me", 20, 20, 20, 1.0);
	cx::DoubleBoundingBox3D originalCroppingBox = image->getCroppingBox();

	cx::DoubleBoundingBox3D explicitBox(2, 8, 2, 8, 2, 8);
	cx::ImagePtr cropped = cx::cropImage(pasm, image, explicitBox);
	REQUIRE(cropped);

	int dims[3];
	cropped->getBaseVtkImageData()->GetDimensions(dims);
	CHECK(dims[0] == 7);
	CHECK(dims[1] == 7);
	CHECK(dims[2] == 7);

	CHECK(cx::similar(image->getCroppingBox(), originalCroppingBox));
}

TEST_CASE("ImageAlgorithms: cropImage()/resampleImageToMaxVoxelCount() use an explicit uid/name as-is", "[unit][resource][core]")
{
	cxtest::TestVisServicesPtr services = cxtest::TestVisServices::create();
	cx::PatientModelServicePtr pasm = services->patient();

	cx::ImagePtr image = createSyntheticImage(pasm, "ct_crop", 20, 20, 20, 1.0);

	cx::ImagePtr cropped = cx::cropImage(pasm, image, cx::DoubleBoundingBox3D(2, 8, 2, 8, 2, 8), "ct_prepared", "CT (prepared)");
	REQUIRE(cropped);
	CHECK(cropped->getUid() == "ct_prepared");
	CHECK(cropped->getName() == "CT (prepared)");

	cx::ImagePtr resampled = cx::resampleImageToMaxVoxelCount(pasm, cropped, 10, "ct_prepared", "CT (prepared)");
	REQUIRE(resampled);
	CHECK(resampled->getUid() == "ct_prepared");
	CHECK(resampled->getName() == "CT (prepared)");
}

TEST_CASE("ImageAlgorithms: resampleImage() alone (no prior crop) preserves physical position", "[unit][resource][core]")
{
	cxtest::TestVisServicesPtr services = cxtest::TestVisServices::create();
	cx::PatientModelServicePtr pasm = services->patient();

	cx::ImagePtr original = createSyntheticImage(pasm, "original2", 20, 20, 20, 1.0);
	cx::Vector3D expectedPhysicalCenter = original->get_rMd().coord(original->boundingBox().center());

	cx::ImagePtr resampled = cx::resampleImage(pasm, original, cx::Vector3D(2.0, 2.0, 2.0));
	REQUIRE(resampled);
	cx::Vector3D resampledPhysicalCenter = resampled->get_rMd().coord(resampled->boundingBox().center());
	CHECK(resampledPhysicalCenter[0] == Approx(expectedPhysicalCenter[0]));
	CHECK(resampledPhysicalCenter[1] == Approx(expectedPhysicalCenter[1]));
	CHECK(resampledPhysicalCenter[2] == Approx(expectedPhysicalCenter[2]));
}

TEST_CASE("ImageAlgorithms: cropImage() then resampleImageToMaxVoxelCount() preserves physical position", "[unit][resource][core]")
{
	cxtest::TestVisServicesPtr services = cxtest::TestVisServices::create();
	cx::PatientModelServicePtr pasm = services->patient();

	cx::ImagePtr original = createSyntheticImage(pasm, "original", 40, 40, 40, 1.0);

	cx::DoubleBoundingBox3D cropBox(5, 24, 5, 24, 5, 24);
	cx::Vector3D expectedPhysicalCenter = original->get_rMd().coord(cropBox.center());

	cx::ImagePtr cropped = cx::cropImage(pasm, original, cropBox);
	REQUIRE(cropped);
	cx::Vector3D croppedPhysicalCenter = cropped->get_rMd().coord(cropped->boundingBox().center());
	CHECK(croppedPhysicalCenter[0] == Approx(expectedPhysicalCenter[0]));
	CHECK(croppedPhysicalCenter[1] == Approx(expectedPhysicalCenter[1]));
	CHECK(croppedPhysicalCenter[2] == Approx(expectedPhysicalCenter[2]));

	cx::ImagePtr resampled = cx::resampleImageToMaxVoxelCount(pasm, cropped, 1000);
	REQUIRE(resampled);
	REQUIRE(resampled != cropped);
	cx::Vector3D resampledPhysicalCenter = resampled->get_rMd().coord(resampled->boundingBox().center());
	CHECK(resampledPhysicalCenter[0] == Approx(expectedPhysicalCenter[0]));
	CHECK(resampledPhysicalCenter[1] == Approx(expectedPhysicalCenter[1]));
	CHECK(resampledPhysicalCenter[2] == Approx(expectedPhysicalCenter[2]));
}

TEST_CASE("ImageAlgorithms: computeAutoCropBox() returns the full volume for a uniform image", "[unit][resource][core]")
{
	cx::ImagePtr image = createSyntheticBodyInAirImage(0, 0);
	cx::DoubleBoundingBox3D box = cx::computeAutoCropBox(image, 0);
	cx::DoubleBoundingBox3D fullVolume = image->boundingBox();
	CHECK(cx::similar(box, fullVolume));
}

namespace
{
/** A synthetic CT volume (20x20 in-plane, 5mm spacing) modeling, along z:
 *   - z[0,9]:    a decoy "arms raised above the head" region - two separate
 *                tissue blocks with a background gap between them that is
 *                NOT lung, but is still open to the slice border above them
 *                (regression test for a real bug: a per-slice bounding-box
 *                check alone flags this gap as false lung-like air, since
 *                it sits inside the tight rectangle around both arms).
 *   - z[10,29]:  "neck" - a narrow tissue block.
 *   - z[30,69]:  "thorax" - a wide tissue block with two genuinely enclosed
 *                "lung" air pockets.
 *   - z[70,149]: "abdomen" - a wide, solid tissue block (no internal air) -
 *                the largest in-body cross-section in the volume.
 */
cx::ImagePtr createSyntheticTorsoImage()
{
	const int dimX = 20, dimY = 20, dimZ = 150;
	vtkSmartPointer<vtkImageData> raw = vtkSmartPointer<vtkImageData>::New();
	raw->SetDimensions(dimX, dimY, dimZ);
	raw->SetSpacing(5.0, 5.0, 5.0);
	raw->AllocateScalars(VTK_SHORT, 1);

	const short background = -1000;
	const short tissue = 40;
	const short lungAir = -800;

	short* ptr = static_cast<short*>(raw->GetScalarPointer());
	vtkIdType idx = 0;
	for (int z = 0; z < dimZ; ++z)
	{
		for (int y = 0; y < dimY; ++y)
		{
			for (int x = 0; x < dimX; ++x, ++idx)
			{
				short value = background;
				if (z <= 9)
				{
					bool armL = (x >= 2 && x <= 7) && (y >= 5 && y <= 17);
					bool armR = (x >= 12 && x <= 17) && (y >= 5 && y <= 17);
					if (armL || armR)
						value = tissue;
				}
				else if (z <= 29)
				{
					if (x >= 7 && x <= 12 && y >= 7 && y <= 12)
						value = tissue;
				}
				else if (z <= 69)
				{
					bool inTorso = (x >= 2 && x <= 17) && (y >= 2 && y <= 17);
					bool inLungL = (x >= 3 && x <= 7) && (y >= 4 && y <= 15);
					bool inLungR = (x >= 12 && x <= 16) && (y >= 4 && y <= 15);
					if (inTorso)
						value = (inLungL || inLungR) ? lungAir : tissue;
				}
				else
				{
					if ((x >= 2 && x <= 17) && (y >= 2 && y <= 17))
						value = tissue;
				}
				ptr[idx] = value;
			}
		}
	}

	// A single, otherwise-irrelevant voxel just above the tissue value:
	// vtkImageAccumulate's bins are half-open, so a population sitting
	// exactly at the volume's max scalar value would otherwise fall outside
	// even the last bin and be silently dropped, corrupting the Otsu split.
	// Real scan data doesn't have this edge case (a continuous distribution
	// near the max), but this synthetic image's single-valued "tissue"
	// spike does.
	ptr[0] = tissue + 1;

	return cx::ImagePtr(new cx::Image("synthetic_torso", raw, "synthetic_torso", cx::imCT));
}

/** A synthetic CT volume that's just a short "head/upper chest" scan with a
 *  genuinely enclosed (not reachable from any slice border) air pocket -
 *  passes both the lung-fraction and minimum-run-length checks, so it looks
 *  like a real lung to computeLungBaseCropBox(), but there is no real
 *  abdomen below it for the designed margin to land in once clamped to the
 *  volume's own short extent.
 */
cx::ImagePtr createSyntheticShortScanWithEnclosedPocketImage()
{
	const int dimX = 20, dimY = 20, dimZ = 30;
	vtkSmartPointer<vtkImageData> raw = vtkSmartPointer<vtkImageData>::New();
	raw->SetDimensions(dimX, dimY, dimZ);
	raw->SetSpacing(5.0, 5.0, 5.0);
	raw->AllocateScalars(VTK_SHORT, 1);

	const short background = -1000;
	const short tissue = 40;
	const short lungAir = -800;

	short* ptr = static_cast<short*>(raw->GetScalarPointer());
	vtkIdType idx = 0;
	for (int z = 0; z < dimZ; ++z)
	{
		for (int y = 0; y < dimY; ++y)
		{
			for (int x = 0; x < dimX; ++x, ++idx)
			{
				short value = background;
				if (z <= 24)
				{
					bool inTorso = (x >= 2 && x <= 17) && (y >= 2 && y <= 17);
					bool inPocket = (x >= 6 && x <= 13) && (y >= 6 && y <= 13);
					if (inTorso)
						value = inPocket ? lungAir : tissue;
				}
				ptr[idx] = value;
			}
		}
	}
	ptr[0] = tissue + 1; // avoid the max-value histogram edge case (see createSyntheticTorsoImage() above)

	return cx::ImagePtr(new cx::Image("synthetic_short_scan", raw, "synthetic_short_scan", cx::imCT));
}
}

TEST_CASE("ImageAlgorithms: computeLungBaseCropBox() discards an implausibly thin result", "[unit][resource][core]")
{
	cx::ImagePtr image = createSyntheticShortScanWithEnclosedPocketImage();
	cx::DoubleBoundingBox3D box = cx::computeLungBaseCropBox(image);
	CHECK(cx::similar(box, image->boundingBox()));
}

TEST_CASE("ImageAlgorithms: computeLungBaseCropBox() finds the diaphragm level, not the decoy arms-above-head gap", "[unit][resource][core]")
{
	cx::ImagePtr image = createSyntheticTorsoImage();
	cx::DoubleBoundingBox3D box = cx::computeLungBaseCropBox(image);
	cx::DoubleBoundingBox3D fullVolume = image->boundingBox();

	CHECK_FALSE(cx::similar(box, fullVolume));
	CHECK(box[4] == Approx(305.0));
	CHECK(box[5] == Approx(625.0));
	CHECK(box[0] == Approx(fullVolume[0]));
	CHECK(box[1] == Approx(fullVolume[1]));
	CHECK(box[2] == Approx(fullVolume[2]));
	CHECK(box[3] == Approx(fullVolume[3]));
}

TEST_CASE("ImageAlgorithms: computeLungBaseCropBox() returns the full volume for non-CT", "[unit][resource][core]")
{
	cx::ImagePtr image = createSyntheticTorsoImage();
	image->setModality(cx::imMR);
	cx::DoubleBoundingBox3D box = cx::computeLungBaseCropBox(image);
	CHECK(cx::similar(box, image->boundingBox()));
}

TEST_CASE("ImageAlgorithms: computeWidestCrossSectionCropBox() centers on the abdomen, the widest cross-section", "[unit][resource][core]")
{
	cx::ImagePtr image = createSyntheticTorsoImage();
	cx::DoubleBoundingBox3D box = cx::computeWidestCrossSectionCropBox(image);
	cx::DoubleBoundingBox3D fullVolume = image->boundingBox();

	CHECK_FALSE(cx::similar(box, fullVolume));
	CHECK(box[4] == Approx(50.0));
	CHECK(box[5] == Approx(650.0));
}

TEST_CASE("ImageAlgorithms: computeAutoCropBoxAbdomen() prefers the lung-based result when it is available", "[unit][resource][core]")
{
	cx::ImagePtr image = createSyntheticTorsoImage();
	cx::DoubleBoundingBox3D box = cx::computeAutoCropBoxAbdomen(image);
	cx::DoubleBoundingBox3D lungBaseBox = cx::computeLungBaseCropBox(image);
	CHECK(cx::similar(box, lungBaseBox));
}

TEST_CASE("ImageAlgorithms: computeAutoCropBoxAbdomen() falls back to the cross-section result for non-CT", "[unit][resource][core]")
{
	cx::ImagePtr image = createSyntheticTorsoImage();
	image->setModality(cx::imMR);
	cx::DoubleBoundingBox3D box = cx::computeAutoCropBoxAbdomen(image);
	cx::DoubleBoundingBox3D crossSectionBox = cx::computeWidestCrossSectionCropBox(image);
	CHECK(cx::similar(box, crossSectionBox));
}

