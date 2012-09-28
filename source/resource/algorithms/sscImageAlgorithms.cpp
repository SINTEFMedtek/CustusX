/*
 * sscImageAlgorithms.cpp
 *
 *  \date Nov 11, 2010
 *      \author christiana
 */
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

typedef vtkSmartPointer<class vtkImageShiftScale> vtkImageShiftScalePtr;

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

  QString uid = image->getUid() + "_crop%1";
  QString name = image->getName()+" crop%1";
  ImagePtr result = dataManager()->createDerivedImage(rawResult,uid, name, image);
  result->mergevtkSettingsIntosscTransform();

/////// Old code
/*  ImagePtr result = dataManager()->createImage(rawResult,uid, name);
  result->get_rMd_History()->setRegistration(image->get_rMd());
  result->mergevtkSettingsIntosscTransform();
  result->resetTransferFunction(image->getTransferFunctions3D()->createCopy(image->getBaseVtkImageData()),
  image->getLookupTable2D()->createCopy(image->getBaseVtkImageData()));
  result->get_rMd_History()->setParentSpace(image->getUid());*/
///////
  //messageManager()->sendInfo("Created volume " + result->getName());

//  dataManager()->loadData(result);
//  dataManager()->saveImage(result, outputBasePath);

//  std::cout << "CROPPED" << std::endl;
//  std::cout << "rMd\n" << result->get_rMd() << std::endl;
//  result->getBaseVtkImageData()->Print(std::cout);

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


/**Convert the input image to the smallest unsigned format.
 *
 * CT images are always shifted +1024 and converted.
 * Other images are shifted so that the smallest intensity
 * is mapped to zero.
 *
 * Either VTK_UNSIGNED_SHORT or VTK_UNSIGNED_INT is used
 * as output, depending on the input range.
 *
 */
vtkImageDataPtr convertImageToUnsigned(ImagePtr image)
{
	vtkImageDataPtr input = image->getBaseVtkImageData();

//	if (input->GetScalarRange()[0] >= 0) // wrong: must convert type even if all data are positive
//		return input;
	if (input->GetScalarTypeMin() >= 0)
		return input;

	vtkImageShiftScalePtr cast = vtkImageShiftScalePtr::New();
	cast->SetInput(input);
	cast->ClampOverflowOn();

	// start by shifting up to zero
	cast->SetShift(-input->GetScalarRange()[0]);

	// if CT: always shift by 1024 (houndsfield units definition)
	if (image->getModality().contains("CT", Qt::CaseInsensitive))
		cast->SetShift(1024);

	// total intensity range of voxels:
	double range = input->GetScalarRange()[1] - input->GetScalarRange()[0];

	// to to fit within smallest type
	if (range <= VTK_UNSIGNED_SHORT_MAX-VTK_UNSIGNED_SHORT_MIN)
		cast->SetOutputScalarType(VTK_UNSIGNED_SHORT);
	else if (range <= VTK_UNSIGNED_INT_MAX-VTK_UNSIGNED_INT_MIN)
		cast->SetOutputScalarType(VTK_UNSIGNED_INT);
//	else if (range <= VTK_UNSIGNED_LONG_MAX-VTK_UNSIGNED_LONG_MIN) // not supported by vtk - it seems (crash in rendering)
//		cast->SetOutputScalarType(VTK_UNSIGNED_LONG);
	else
		cast->SetOutputScalarType(VTK_UNSIGNED_INT);

	cast->Update();
	ssc::messageManager()->sendInfo(QString("Converting image %1 from %2 to %3").arg(image->getName()).arg(input->GetScalarTypeAsString()).arg(cast->GetOutput()->GetScalarTypeAsString()));
	return cast->GetOutput();
}


} // namespace ssc
