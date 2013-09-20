#include "cxTest_ImageAlgorithms.h"

#include <vtkImageData.h>
#include "sscDataManager.h"
#include "sscImage.h"
#include "sscImageAlgorithms.h"
#include "sscImageTF3D.h"
#include "sscImageLUT2D.h"
#include "cxDataLocations.h"

//#include "cxToolConfigurationParser.h"

void TestImageAlgorithms::setUp()
{
  // this stuff will be performed just before all tests in this class
}

void TestImageAlgorithms::tearDown()
{
  // this stuff will be performed just after all tests in this class
}

/**
 * Simple tests for the code used by Resample::calculate() in cxResample
 */
void TestImageAlgorithms::testResample()
{
	QString fname0 = cx::DataLocations::getTestDataPath() + "/testing/ResampleTest.cx3/Images/mra.mhd";
	QString fname1 = cx::DataLocations::getTestDataPath() + "/testing/ResampleTest.cx3/Images/US_01_20110222T110117_1.mhd";

	/*DataPtr image = */cx::dataManager()->loadData(fname0, fname0, cx::rtAUTO);
	/*DataPtr referenceImage = */cx::dataManager()->loadData(fname1, fname1, cx::rtAUTO);
	cx::ImagePtr image = cx::dataManager()->getImage(fname0);
	cx::ImagePtr referenceImage = cx::dataManager()->getImage(fname1);
//	std::cout << "referenceImage base: " << referenceImage->getBaseVtkImageData() << std::endl;
	CPPUNIT_ASSERT(image!=0);
	CPPUNIT_ASSERT(referenceImage!=0);

	cx::Transform3D refMi = referenceImage->get_rMd().inv() * image->get_rMd();

  cx::ImagePtr oriented = resampleImage(image, refMi);
	CPPUNIT_ASSERT(oriented!=0);
  int inMin = image->getBaseVtkImageData()->GetScalarRange()[0];
  int inMax = image->getBaseVtkImageData()->GetScalarRange()[1];
  int outMin = oriented->getBaseVtkImageData()->GetScalarRange()[0];
  int outMax = oriented->getBaseVtkImageData()->GetScalarRange()[1];
  CPPUNIT_ASSERT(inMin == outMin);
//    std::cout << "inMax: " << inMax << " outMax: " << outMax << std::endl;
  CPPUNIT_ASSERT(inMax == outMax);
  CPPUNIT_ASSERT(image->getBaseVtkImageData() != oriented->getBaseVtkImageData());
  CPPUNIT_ASSERT(image->getTransferFunctions3D()->getVtkImageData() == image->getBaseVtkImageData());
//  std::cout << "image:    " << image->getBaseVtkImageData() << " oriented:    " << oriented->getBaseVtkImageData() << std::endl;
//  std::cout << "image tf: " << image->getTransferFunctions3D()->getVtkImageData() << " oriented tf: " << oriented->getTransferFunctions3D()->getVtkImageData() << std::endl;
  //Make sure the image and tf points to the same vtkImageData
  CPPUNIT_ASSERT(oriented->getTransferFunctions3D()->getVtkImageData() == oriented->getBaseVtkImageData());
  CPPUNIT_ASSERT(oriented->getLookupTable2D()->getVtkImageData() == oriented->getBaseVtkImageData());

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
  CPPUNIT_ASSERT(cropped!=0);
  int cropMin = cropped->getBaseVtkImageData()->GetScalarRange()[0];
  int cropMax = cropped->getBaseVtkImageData()->GetScalarRange()[1];
  CPPUNIT_ASSERT(cropMin == inMin);
  CPPUNIT_ASSERT(cropMax >  inMin);
  CPPUNIT_ASSERT(cropMax <= inMax);
  CPPUNIT_ASSERT(oriented->getBaseVtkImageData() != cropped->getBaseVtkImageData());
  CPPUNIT_ASSERT(cropped->getTransferFunctions3D()->getVtkImageData() == cropped->getBaseVtkImageData());
  CPPUNIT_ASSERT(cropped->getLookupTable2D()->getVtkImageData() == cropped->getBaseVtkImageData());

  QString uid = image->getUid() + "_resample%1";
  QString name = image->getName() + " resample%1";

  cx::ImagePtr resampled = cx::resampleImage(cropped, cx::Vector3D(referenceImage->getBaseVtkImageData()->GetSpacing()), uid, name);
	CPPUNIT_ASSERT(resampled!=0);
  outMin = resampled->getBaseVtkImageData()->GetScalarRange()[0];
  outMax = resampled->getBaseVtkImageData()->GetScalarRange()[1];
//  std::cout << "outMin: " << outMin << " cropMin: " << cropMin << std::endl;
//  CPPUNIT_ASSERT(outMin == cropMin); //What happens when min == 1 instead of 0? //Looks ok in ImageTFData::fixTransferFunctions()
  CPPUNIT_ASSERT(outMin >=  cropMin);
  CPPUNIT_ASSERT(outMax >  cropMin);
  CPPUNIT_ASSERT(outMax <= cropMax);
  CPPUNIT_ASSERT(outMax <= cropMax);
  CPPUNIT_ASSERT(cropped->getBaseVtkImageData() != resampled->getBaseVtkImageData());
  CPPUNIT_ASSERT(resampled->getTransferFunctions3D()->getVtkImageData() == resampled->getBaseVtkImageData());
  CPPUNIT_ASSERT(resampled->getLookupTable2D()->getVtkImageData() == resampled->getBaseVtkImageData());

}
