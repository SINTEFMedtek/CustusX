/*
 * sscImageAlgorithms.cpp
 *
 *  Created on: Nov 11, 2010
 *      Author: christiana
 */
#include "sscImageAlgorithms.h"

#include <vtkImageData.h>
#include <vtkImageReslice.h>
#include <vtkMatrix4x4.h>

#include <vtkImageResample.h>
#include <vtkImageClip.h>

#include "sscImage.h"
#include "sscDataManager.h"
#include "sscUtilHelpers.h"
#include "sscImageTF3D.h"
#include "sscImageLUT2D.h"
#include "sscRegistrationTransform.h"


namespace ssc
{

/** Return an image that is resampled into space q.
 *  The image is not added to the data manager nor saved.
 */
ssc::ImagePtr resampleImage(ssc::ImagePtr image, ssc::Transform3D qMd)
{
  // provide a resampled volume for algorithms requiring that (such as proberep)
  vtkMatrix4x4Ptr orientatorMatrix = vtkMatrix4x4Ptr::New();
  vtkImageReslicePtr orientator = vtkImageReslicePtr::New();
  orientator->SetInput(image->getBaseVtkImageData());
  orientator->SetInterpolationModeToLinear();
  orientator->SetOutputDimensionality(3);
  orientator->SetResliceAxes(qMd.inv().matrix());
  orientator->AutoCropOutputOn();
  vtkImageDataPtr rawResult = orientator->GetOutput();

  rawResult->Update();
//  rawResult->Print(std::cout);

  QString uid = ssc::changeExtension(image->getUid(), "") + "_or%1";
  QString name = image->getName()+" or%1";
  ssc::ImagePtr oriented = ssc::dataManager()->createImage(rawResult, uid, name);
  //oriented->get_rMd_History()->setRegistration(reference->get_rMd());
  oriented->get_rMd_History()->setRegistration(image->get_rMd() * qMd.inv());
//  std::cout << "rMd pre merge oriented\n" << oriented->get_rMd() << std::endl;
  oriented->mergevtkSettingsIntosscTransform();
//  std::cout << "rMd finished oriented\n" << oriented->get_rMd() << std::endl;
  oriented->resetTransferFunction(image->getTransferFunctions3D()->createCopy(), image->getLookupTable2D()->createCopy());
  oriented->get_rMd_History()->addParentFrame(image->getUid());

  return oriented;
}

/** Return an image that is resampled with a new output spacing.
 *  The image is not added to the data manager nor saved.
 */
ImagePtr resampleImage(ssc::ImagePtr image, const Vector3D spacing, QString uid, QString name)
{
  std::cout << "oldspacing: " << ssc::Vector3D(image->getBaseVtkImageData()->GetSpacing()) << std::endl;
  std::cout << "spacing: " << spacing << std::endl;
  vtkImageResamplePtr resampler = vtkImageResamplePtr::New();
  resampler->SetInput(image->getBaseVtkImageData());
  resampler->SetAxisOutputSpacing(0, spacing[0]);
  resampler->SetAxisOutputSpacing(1, spacing[1]);
  resampler->SetAxisOutputSpacing(2, spacing[2]);
  vtkImageDataPtr rawResult = resampler->GetOutput();

  rawResult->Update();

  if (uid.isEmpty())
  {
    uid = ssc::changeExtension(image->getUid(), "") + "_res%1";
    name = image->getName()+" res%1";
  }
  ssc::ImagePtr retval = ssc::dataManager()->createImage(rawResult, uid, name);
  retval->get_rMd_History()->setRegistration(image->get_rMd());
  retval->resetTransferFunction(image->getTransferFunctions3D()->createCopy(), image->getLookupTable2D()->createCopy());
  retval->get_rMd_History()->addParentFrame(image->getUid());

  //ssc::dataManager()->loadData(retval);
  //ssc::dataManager()->saveImage(retval, outputBasePath);
  return retval;
}

/** Return an image that is cropped using its own croppingBox.
 *  The image is not added to the data manager nor saved.
 */
ImagePtr cropImage(ImagePtr image)
{
  vtkImageClipPtr clip = vtkImageClipPtr::New();
  DoubleBoundingBox3D bb = image->getCroppingBox();
  clip->SetInput(image->getBaseVtkImageData());

  double* sp = image->getBaseVtkImageData()->GetSpacing();

  clip->SetOutputWholeExtent(
      static_cast<int>(bb[0]/sp[0]+0.5), static_cast<int>(bb[1]/sp[1]+0.5),
      static_cast<int>(bb[2]/sp[1]+0.5), static_cast<int>(bb[3]/sp[1]+0.5),
      static_cast<int>(bb[4]/sp[2]+0.5), static_cast<int>(bb[5]/sp[2]+0.5));

  clip->ClipDataOn();
  vtkImageDataPtr rawResult = clip->GetOutput();

  rawResult->Update();
  rawResult->UpdateInformation();
  rawResult->ComputeBounds();

//  return retVal;


//  this->getBaseVtkImageData()->Update();
//  this->getBaseVtkImageData()->UpdateInformation();
//  this->getBaseVtkImageData()->Print(std::cout);

//  vtkImageDataPtr rawResult = this->CropAndClipImageTovtkImageData();

  QString uid = changeExtension(image->getUid(), "") + "_crop%1";
  QString name = image->getName()+" crop%1";
  ImagePtr result = dataManager()->createImage(rawResult,uid, name);
  result->get_rMd_History()->setRegistration(image->get_rMd());
  result->mergevtkSettingsIntosscTransform();
  result->resetTransferFunction(image->getTransferFunctions3D()->createCopy(), image->getLookupTable2D()->createCopy());
  result->get_rMd_History()->addParentFrame(image->getUid());
  //messageManager()->sendInfo("Created volume " + result->getName());

//  dataManager()->loadData(result);
//  dataManager()->saveImage(result, outputBasePath);

//  std::cout << "CROPPED" << std::endl;
//  std::cout << "rMd\n" << result->get_rMd() << std::endl;
//  result->getBaseVtkImageData()->Print(std::cout);

  return result;

}


} // namespace ssc
