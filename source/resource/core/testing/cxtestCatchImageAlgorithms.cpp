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

	// Should adapt to a shifted (unsigned) representation just as well -
	// same relative split, different absolute values.
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

	// Padding should clamp at the volume edge rather than exceed it.
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

	cx::ImagePtr image = createSyntheticImage(pasm, "big", 40, 40, 40, 1.0); // 64000 voxels

	cx::ImagePtr resampled = cx::resampleImageToMaxVoxelCount(pasm, image, 8000);
	REQUIRE(resampled);
	CHECK(resampled != image);

	int dims[3];
	resampled->getBaseVtkImageData()->GetDimensions(dims);
	double voxelCount = static_cast<double>(dims[0]) * dims[1] * dims[2];
	CHECK(voxelCount <= 8000 * 1.5); // some slack for integer dimension rounding
}

TEST_CASE("ImageAlgorithms: resampleImageToMaxVoxelCount() is a no-op under the target", "[unit][resource][core]")
{
	cxtest::TestVisServicesPtr services = cxtest::TestVisServices::create();
	cx::PatientModelServicePtr pasm = services->patient();

	cx::ImagePtr image = createSyntheticImage(pasm, "small", 10, 10, 10, 1.0); // 1000 voxels

	cx::ImagePtr resampled = cx::resampleImageToMaxVoxelCount(pasm, image, 8000);
	CHECK(resampled == image); // same object - no resampling performed
}

TEST_CASE("ImageAlgorithms: resampleImageToMaxInPlaneResolution() caps x/y and keeps z spacing", "[unit][resource][core]")
{
	cxtest::TestVisServicesPtr services = cxtest::TestVisServices::create();
	cx::PatientModelServicePtr pasm = services->patient();

	cx::ImagePtr image = createSyntheticImage(pasm, "wide", 40, 40, 10, 1.0);

	cx::ImagePtr resampled = cx::resampleImageToMaxInPlaneResolution(pasm, image, 20);
	REQUIRE(resampled);
	double* spacing = resampled->getBaseVtkImageData()->GetSpacing();
	CHECK(spacing[0] == Approx(2.0)); // 40/20 * 1.0
	CHECK(spacing[1] == Approx(2.0));
	CHECK(spacing[2] == Approx(1.0)); // z untouched
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
	CHECK(dims[0] == 7); // voxel indices 2..8 inclusive
	CHECK(dims[1] == 7);
	CHECK(dims[2] == 7);

	CHECK(cx::similar(image->getCroppingBox(), originalCroppingBox));
}

TEST_CASE("ImageAlgorithms: computeAutoCropBox() returns the full volume for a uniform image", "[unit][resource][core]")
{
	// No separable foreground/background - computeOtsuThreshold() degenerates
	// to range[0], under which every voxel counts as foreground, so the
	// result should still cover the whole volume (rather than, say, an
	// empty or otherwise invalid box).
	cx::ImagePtr image = createSyntheticBodyInAirImage(0, 0);
	cx::DoubleBoundingBox3D box = cx::computeAutoCropBox(image, 0);
	cx::DoubleBoundingBox3D fullVolume = image->boundingBox();
	CHECK(cx::similar(box, fullVolume));
}

