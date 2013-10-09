// This file is part of CustusX, an Image Guided Therapy Application.
//
// Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
//
// CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
// code and binaries can only be used by SMT and those with explicit permission
// from SMT. CustusX shall not be distributed to anyone else.
//
// CustusX is a research tool. It is NOT intended for use or certified for use
// in a normal clinical setting. SMT does not take responsibility for its use
// in any way.
//
// See CustusX_License.txt for more information.

#include "catch.hpp"
#include "sscImageAlgorithms.h"

#include <vtkImageData.h>
#include "sscDataManager.h"
#include "sscImage.h"
#include "sscImageTF3D.h"
#include "sscImageLUT2D.h"
#include "cxDataLocations.h"


using namespace cx;

TEST_CASE("ImageAlgorithms: resample() works", "[unit][resource][core]")
{
	QString fname0 = cx::DataLocations::getTestDataPath() + "/testing/ResampleTest.cx3/Images/mra.mhd";
	QString fname1 = cx::DataLocations::getTestDataPath() + "/testing/ResampleTest.cx3/Images/US_01_20110222T110117_1.mhd";

	/*DataPtr image = */cx::dataManager()->loadData(fname0, fname0, cx::rtAUTO);
	/*DataPtr referenceImage = */cx::dataManager()->loadData(fname1, fname1, cx::rtAUTO);
	cx::ImagePtr image = cx::dataManager()->getImage(fname0);
	cx::ImagePtr referenceImage = cx::dataManager()->getImage(fname1);
	//	std::cout << "referenceImage base: " << referenceImage->getBaseVtkImageData() << std::endl;
	REQUIRE(image);
	REQUIRE(referenceImage);

	cx::Transform3D refMi = referenceImage->get_rMd().inv() * image->get_rMd();

	cx::ImagePtr oriented = resampleImage(image, refMi);
	REQUIRE(oriented);
	int inMin = image->getBaseVtkImageData()->GetScalarRange()[0];
	int inMax = image->getBaseVtkImageData()->GetScalarRange()[1];
	int outMin = oriented->getBaseVtkImageData()->GetScalarRange()[0];
	int outMax = oriented->getBaseVtkImageData()->GetScalarRange()[1];
	CHECK(inMin == outMin);
	//    std::cout << "inMax: " << inMax << " outMax: " << outMax << std::endl;
	CHECK(inMax == outMax);
	CHECK(image->getBaseVtkImageData() != oriented->getBaseVtkImageData());
	CHECK(image->getTransferFunctions3D()->getVtkImageData() == image->getBaseVtkImageData());
	//  std::cout << "image:    " << image->getBaseVtkImageData() << " oriented:    " << oriented->getBaseVtkImageData() << std::endl;
	//  std::cout << "image tf: " << image->getTransferFunctions3D()->getVtkImageData() << " oriented tf: " << oriented->getTransferFunctions3D()->getVtkImageData() << std::endl;
	//Make sure the image and tf points to the same vtkImageData
	CHECK(oriented->getTransferFunctions3D()->getVtkImageData() == oriented->getBaseVtkImageData());
	CHECK(oriented->getLookupTable2D()->getVtkImageData() == oriented->getBaseVtkImageData());

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

	cx::ImagePtr cropped = cropImage(oriented);
	REQUIRE(cropped);
	int cropMin = cropped->getBaseVtkImageData()->GetScalarRange()[0];
	int cropMax = cropped->getBaseVtkImageData()->GetScalarRange()[1];
	CHECK(cropMin == inMin);
	CHECK(cropMax >  inMin);
	CHECK(cropMax <= inMax);
	CHECK(oriented->getBaseVtkImageData() != cropped->getBaseVtkImageData());
	CHECK(cropped->getTransferFunctions3D()->getVtkImageData() == cropped->getBaseVtkImageData());
	CHECK(cropped->getLookupTable2D()->getVtkImageData() == cropped->getBaseVtkImageData());

	QString uid = image->getUid() + "_resample%1";
	QString name = image->getName() + " resample%1";

	cx::ImagePtr resampled = cx::resampleImage(cropped, cx::Vector3D(referenceImage->getBaseVtkImageData()->GetSpacing()), uid, name);
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
	CHECK(resampled->getTransferFunctions3D()->getVtkImageData() == resampled->getBaseVtkImageData());
	CHECK(resampled->getLookupTable2D()->getVtkImageData() == resampled->getBaseVtkImageData());

}

