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

#include "sscImageAlgorithms.h"

#include <vtkImageData.h>
#include <vtkImageReslice.h>
#include <vtkMatrix4x4.h>

#include <vtkImageResample.h>
#include <vtkImageClip.h>
#include "vtkImageShiftScale.h"

#include "sscImage.h"
#include "sscDataManager.h"
#include "sscUtilHelpers.h"
#include "sscImageTF3D.h"
#include "sscImageLUT2D.h"
#include "sscRegistrationTransform.h"
#include "sscMessageManager.h"
#include "sscTime.h"


namespace ssc
{

/** Return an image that is resampled into space q.
 *  The image is not added to the data manager nor saved.
 */
ssc::ImagePtr resampleImage(ssc::ImagePtr image, ssc::Transform3D qMd)
{
	//TODO: fix error:
	// There is an error in the transfer functions of the returned image from this function

  // provide a resampled volume for algorithms requiring that (such as PickerRep)
  vtkMatrix4x4Ptr orientatorMatrix = vtkMatrix4x4Ptr::New();
  vtkImageReslicePtr orientator = vtkImageReslicePtr::New();
  orientator->SetInput(image->getBaseVtkImageData());
  orientator->SetInterpolationModeToLinear();
  orientator->SetOutputDimensionality(3);
  orientator->SetResliceAxes(qMd.inv().getVtkMatrix());
  orientator->AutoCropOutputOn();
  vtkImageDataPtr rawResult = orientator->GetOutput();

  rawResult->Update();
//  rawResult->Print(std::cout);

  QString uid = image->getUid() + "_or%1";
  QString name = image->getName()+" or%1";
  ssc::ImagePtr oriented = ssc::dataManager()->createDerivedImage(rawResult, uid, name, image);
  //oriented->get_rMd_History()->setRegistration(reference->get_rMd());
  oriented->get_rMd_History()->setRegistration(image->get_rMd() * qMd.inv());
//  std::cout << "rMd pre merge oriented\n" << oriented->get_rMd() << std::endl;
  oriented->mergevtkSettingsIntosscTransform();
//  std::cout << "rMd finished oriented\n" << oriented->get_rMd() << std::endl;

  return oriented;
}

/** Return an image that is resampled with a new output spacing.
 *  The image is not added to the data manager nor saved.
 */
ImagePtr resampleImage(ssc::ImagePtr image, const Vector3D spacing, QString uid, QString name)
{
//  std::cout << "oldspacing: " << ssc::Vector3D(image->getBaseVtkImageData()->GetSpacing()) << std::endl;
//  std::cout << "spacing: " << spacing << std::endl;
  vtkImageResamplePtr resampler = vtkImageResamplePtr::New();
  resampler->SetInput(image->getBaseVtkImageData());
  resampler->SetAxisOutputSpacing(0, spacing[0]);
  resampler->SetAxisOutputSpacing(1, spacing[1]);
  resampler->SetAxisOutputSpacing(2, spacing[2]);
  vtkImageDataPtr rawResult = resampler->GetOutput();

  rawResult->Update();

  if (uid.isEmpty())
  {
    uid = image->getUid() + "_res%1";
    name = image->getName()+" res%1";
  }
  ssc::ImagePtr retval = ssc::dataManager()->createDerivedImage(rawResult, uid, name, image);

  //ssc::dataManager()->loadData(retval);
  //ssc::dataManager()->saveImage(retval, outputBasePath);
  return retval;
}

/** Return an image that is cropped using its own croppingBox.
 *  The image is not added to the data manager nor saved.
 */
ImagePtr duplicateImage(ImagePtr image)
{
	ssc::Vector3D spacing(image->getBaseVtkImageData()->GetSpacing());
	return resampleImage(image, spacing, image->getUid()+"_copy%1", image->getName()+" copy%1");
}

/** Return an image that is cropped using its own croppingBox.
 *  The image is not added to the data manager nor saved.
 */
vtkImageDataPtr cropImage(vtkImageDataPtr input, IntBoundingBox3D cropbox)
{
  vtkImageClipPtr clip = vtkImageClipPtr::New();
  clip->SetInput(input);
  clip->SetOutputWholeExtent(cropbox.begin());
  clip->ClipDataOn();
  vtkImageDataPtr rawResult = clip->GetOutput();

  rawResult->Update();
  rawResult->UpdateInformation();
  rawResult->ComputeBounds();
  return rawResult;
}

/** Return an image that is cropped using its own croppingBox.
 *  The image is not added to the data manager nor saved.
 */
ImagePtr cropImage(ImagePtr image)
{
  DoubleBoundingBox3D bb = image->getCroppingBox();
  double* sp = image->getBaseVtkImageData()->GetSpacing();
  IntBoundingBox3D cropbox(
	      static_cast<int>(bb[0]/sp[0]+0.5), static_cast<int>(bb[1]/sp[0]+0.5),
	      static_cast<int>(bb[2]/sp[1]+0.5), static_cast<int>(bb[3]/sp[1]+0.5),
	      static_cast<int>(bb[4]/sp[2]+0.5), static_cast<int>(bb[5]/sp[2]+0.5));
  vtkImageDataPtr rawResult = cropImage(image->getBaseVtkImageData(), cropbox);

  QString uid = image->getUid() + "_crop%1";
  QString name = image->getName()+" crop%1";
  ImagePtr result = dataManager()->createDerivedImage(rawResult,uid, name, image);
  result->mergevtkSettingsIntosscTransform();

  return result;
}

/**
 */
QDateTime extractTimestamp(QString text)
{
  // retrieve timestamp as
  QRegExp tsReg("[0-9]{8}T[0-9]{6}");
  if (tsReg.indexIn(text)>0)
  {
    QDateTime datetime = QDateTime::fromString(tsReg.cap(0), ssc::timestampSecondsFormat());
    return datetime;
  }
  return QDateTime();
}




} // namespace ssc
