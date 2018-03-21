/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxImageAlgorithms.h"

#include <vtkImageData.h>
#include <vtkImageReslice.h>
#include <vtkMatrix4x4.h>

#include <vtkImageResample.h>
#include <vtkImageClip.h>

#include "cxImage.h"
#include "cxPatientModelService.h"
#include "cxUtilHelpers.h"
#include "cxImageTF3D.h"
#include "cxImageLUT2D.h"
#include "cxRegistrationTransform.h"

#include "cxTime.h"
#include "cxVolumeHelpers.h"


namespace cx
{

/** Return an image that is resampled into space q.
 *  The image is not added to the data manager nor saved.
 */
ImagePtr resampleImage(PatientModelServicePtr dataManager, ImagePtr image, Transform3D qMd)
{
	//TODO: fix error:
	// There is an error in the transfer functions of the returned image from this function

  // provide a resampled volume for algorithms requiring that (such as PickerRep)
  vtkMatrix4x4Ptr orientatorMatrix = vtkMatrix4x4Ptr::New();
  vtkImageReslicePtr orientator = vtkImageReslicePtr::New();
  orientator->SetInputData(image->getBaseVtkImageData());
  orientator->SetInterpolationModeToLinear();
  orientator->SetOutputDimensionality(3);
  orientator->SetResliceAxes(qMd.inv().getVtkMatrix());
  orientator->AutoCropOutputOn();
  orientator->Update();
  vtkImageDataPtr rawResult = orientator->GetOutput();

//  rawResult->Update();

  QString uid = image->getUid() + "_or%1";
  QString name = image->getName()+" or%1";
//  ImagePtr oriented = dataManager->createDerivedImage(rawResult, uid, name, image);

  ImagePtr oriented = createDerivedImage(dataManager,
									   uid, name,
									   rawResult, image);

  oriented->get_rMd_History()->setRegistration(image->get_rMd() * qMd.inv());
  oriented->mergevtkSettingsIntosscTransform();

  return oriented;
}

/** Return an image that is resampled with a new output spacing.
 *  The image is not added to the data manager nor saved.
 */
ImagePtr resampleImage(PatientModelServicePtr dataManager, ImagePtr image, const Vector3D spacing, QString uid, QString name)
{
  vtkImageResamplePtr resampler = vtkImageResamplePtr::New();
  resampler->SetInputData(image->getBaseVtkImageData());
  resampler->SetAxisOutputSpacing(0, spacing[0]);
  resampler->SetAxisOutputSpacing(1, spacing[1]);
  resampler->SetAxisOutputSpacing(2, spacing[2]);
  resampler->Update();
  vtkImageDataPtr rawResult = resampler->GetOutput();

  if (uid.isEmpty())
  {
    uid = image->getUid() + "_res%1";
    name = image->getName()+" res%1";
  }

  ImagePtr retval = createDerivedImage(dataManager,
									   uid, name,
									   rawResult, image);
  return retval;
}

/** Return an image that is cropped using its own croppingBox.
 *  The image is not added to the data manager nor saved.
 */
ImagePtr duplicateImage(PatientModelServicePtr dataManager, ImagePtr image)
{
	Vector3D spacing(image->getBaseVtkImageData()->GetSpacing());
	return resampleImage(dataManager, image, spacing, image->getUid()+"_copy%1", image->getName()+" copy%1");
}

/** Return an image that is cropped using its own croppingBox.
 *  The image is not added to the data manager nor saved.
 */
vtkImageDataPtr cropImage(vtkImageDataPtr input, IntBoundingBox3D cropbox)
{
  vtkImageClipPtr clip = vtkImageClipPtr::New();
  clip->SetInputData(input);
  clip->SetOutputWholeExtent(cropbox.begin());
  clip->ClipDataOn();
  clip->Update();
  vtkImageDataPtr rawResult = clip->GetOutput();

//  rawResult->Update();
//  rawResult->UpdateInformation();
  rawResult->ComputeBounds();
  return rawResult;
}

/** Return an image that is cropped using its own croppingBox.
 *  The image is not added to the data manager nor saved.
 */
ImagePtr cropImage(PatientModelServicePtr dataManager, ImagePtr image)
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
  ImagePtr result = createDerivedImage(dataManager,
									   uid, name,
									   rawResult, image);
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
    QDateTime datetime = QDateTime::fromString(tsReg.cap(0), timestampSecondsFormat());
    return datetime;
  }
  return QDateTime();
}




} // namespace cx
