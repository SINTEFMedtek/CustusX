#include "cxSegmentation.h"

//ITK
#include <itkImage.h>
#include <itkSmoothingRecursiveGaussianImageFilter.h>
#include <itkBinaryThresholdImageFilter.h>
#include <itkBinaryThinningImageFilter3D.h>
#include "ItkVtkGlue/itkImageToVTKImageFilter.h"
#include "ItkVtkGlue/itkVTKImageToImageFilter.h"

//VTK
#include <vtkSmartPointer.h>
#include <vtkImageData.h>
#include <vtkMarchingCubes.h>
#include <vtkPolyData.h>
#include <vtkImageShrink3D.h>
#include <vtkWindowedSincPolyDataFilter.h>
#include <vtkTriangleFilter.h>
#include <vtkDecimatePro.h>
#include <vtkPolyDataNormals.h>
#include <vtkTubeFilter.h>
#include <vtkImageCast.h>
#include <vtkImageReslice.h>
#include <vtkMatrix4x4.h>
#include <vtkImageResample.h>
#include <vtkImageClip.h>

#include "sscTypeConversions.h"
#include "sscRegistrationTransform.h"
#include "sscImage.h"
#include "sscDataManager.h"
#include "sscUtilHelpers.h"
#include "sscMesh.h"
#include "sscMessageManager.h"
#include "vtkForwardDeclarations.h"
#include "sscImageTF3D.h"
#include "sscImageLUT2D.h"
#include "sscImageAlgorithms.h"

const unsigned int Dimension = 3;
typedef unsigned short PixelType;
typedef itk::Image< PixelType, Dimension >  itkImageType;
typedef itk::ImageToVTKImageFilter<itkImageType> itkToVtkFilterType;
typedef itk::VTKImageToImageFilter<itkImageType> itkVTKImageToImageFilterType;


namespace cx
{

itkImageType::ConstPointer getITKfromSSCImage(ssc::ImagePtr image)
{
  if(!image)
  {
    std::cout << "getITKfromSSCImage(): NO image!!!" << std::endl;
    return itkImageType::ConstPointer();
  }
  itkVTKImageToImageFilterType::Pointer vtk2itkFilter = itkVTKImageToImageFilterType::New();
  //itkToVtkFilter->SetInput(data);
  vtkImageDataPtr input = image->getBaseVtkImageData();
  if (input->GetScalarType() != VTK_UNSIGNED_SHORT)
  //if (input->GetScalarType() == VTK_UNSIGNED_CHAR)
  {
    // convert
    // May need to use vtkImageShiftScale instead if we got data types other than unsigned char?
    vtkImageCastPtr imageCast = vtkImageCastPtr::New();
    imageCast->SetInput(input);
    imageCast->SetOutputScalarTypeToUnsignedShort();
    input = imageCast->GetOutput();
  }
  vtk2itkFilter->SetInput(input);
  vtk2itkFilter->Update();
  return vtk2itkFilter->GetOutput();
}

ssc::MeshPtr Segmentation::contour(ssc::ImagePtr image, QString outputBasePath, int threshold,
    double decimation, bool reduceResolution, bool smoothing)
{
  ssc::messageManager()->sendDebug("Contour, threshold: "+qstring_cast(threshold)+", decimation: "+qstring_cast(decimation)+", reduce resolution: "+qstring_cast(reduceResolution)+", smoothing: "+qstring_cast(smoothing));

  //itkImageType::ConstPointer itkImage = getITKfromSSCImage(image);

    //Create vtkPolyData
  /*vtkImageToPolyDataFilter* convert = vtkImageToPolyDataFilter::New();
   convert->SetInput(itkToVtkFilter->GetOutput());
   convert->SetColorModeToLinear256();
   convert->Update();*/

  //Shrink input volume
  vtkImageShrink3DPtr shrinker = vtkImageShrink3DPtr::New();
  if(reduceResolution)
  {
    ssc::messageManager()->sendInfo("Shrinking volume to be contoured...");
    shrinker->SetInput(image->getBaseVtkImageData());
    shrinker->SetShrinkFactors(2,2,2);
    shrinker->Update();
  }

  // Find countour
  ssc::messageManager()->sendInfo("Finding surface shape...");
  vtkMarchingCubesPtr convert = vtkMarchingCubesPtr::New();
  if(reduceResolution)
    convert->SetInput(shrinker->GetOutput());
  else
    convert->SetInput(image->getBaseVtkImageData());
  //convert->ComputeNormalsOn();
  convert->SetValue(0, threshold);
  //convert->SetValue(0, 1);
  convert->Update();
  //messageManager()->sendInfo("Number of contours: "+QString::number(convert->GetNumberOfContours()).toStdString());

  vtkPolyDataPtr cubesPolyData = vtkPolyDataPtr::New();
  cubesPolyData = convert->GetOutput();
  //cubesPolyData->DeepCopy(convert->GetOutput());

  // Smooth surface model
  vtkWindowedSincPolyDataFilterPtr smoother = vtkWindowedSincPolyDataFilterPtr::New();
  if(smoothing)
  {
    ssc::messageManager()->sendInfo("Smoothing surface...");
    smoother->SetInput(cubesPolyData);
    smoother->Update();
    cubesPolyData = smoother->GetOutput();
  }

  //Create a surface of triangles

  //Decimate surface model (remove a percentage of the polygons)
  vtkTriangleFilterPtr trifilt = vtkTriangleFilterPtr::New();
  vtkDecimateProPtr deci = vtkDecimateProPtr::New();
  vtkPolyDataNormalsPtr normals = vtkPolyDataNormalsPtr::New();
  if (decimation > 0.000001)
  {
    ssc::messageManager()->sendInfo("Creating surface triangles...");
    trifilt->SetInput(cubesPolyData);
    trifilt->Update();
    ssc::messageManager()->sendInfo("Decimating surface...");
    deci->SetInput(trifilt->GetOutput());
    deci->SetTargetReduction(decimation);
    deci->PreserveTopologyOff();
    deci->Update();
    cubesPolyData = deci->GetOutput();
  }

  normals->SetInput(cubesPolyData);
  normals->Update();
//  cubesPolyData = normals->GetOutput();
  //normals->GetOutput()->ReleaseDataFlagOn();// Test: see if this release more memory
  cubesPolyData->DeepCopy(normals->GetOutput());

  //cubesPolyData->Print(std::cout);

  //    vtkImageDataPtr rawResult = vtkImageDataPtr::New();
  //    rawResult->DeepCopy(itkToVtkFilter->GetOutput());
  //    // TODO: possible memory problem here - check debug mem system of itk/vtk


  QString uid = ssc::changeExtension(image->getUid(), "") + "_cont%1";
  QString name = image->getName() + " contour %1";
  //std::cout << "contoured volume: " << uid << ", " << name << std::endl;
  ssc::MeshPtr result = ssc::dataManager()->createMesh(cubesPolyData, uid,
      name, "Images");
  ssc::messageManager()->sendInfo("created contour " + result->getName());

  result->get_rMd_History()->setRegistration(image->get_rMd());
  result->setParentFrame(image->getUid());

  ssc::dataManager()->loadData(result);
  ssc::dataManager()->saveMesh(result, outputBasePath);

  return result;
  //
  //    //print
  //    //itkToVtkFilter->GetOutput()->Print(std::cout);
  //    //cubesPolyData->Print(std::cout);
  //    //vtkPolyData* cubesPolyData = convert->GetOutput();
  //
  //    ssc::MeshPtr surface = ssc::MeshPtr(new ssc::Mesh(outName.toStdString()+"_segm"));
  //    surface->setVtkPolyData(cubesPolyData);
  //    ssc::GeometricRepPtr surfaceRep(ssc::GeometricRep::New(outName.toStdString()+"_segm"));
  //    surfaceRep->setMesh(surface);

}

ssc::ImagePtr Segmentation::segment(ssc::ImagePtr image, QString outputBasePath, int threshold, bool useSmothing, double smoothSigma)
{
  itkImageType::ConstPointer itkImage = getITKfromSSCImage(image);

  //Smoothing
  if(useSmothing)
  {
    ssc::messageManager()->sendDebug("Smoothing...");
    typedef itk::SmoothingRecursiveGaussianImageFilter<itkImageType, itkImageType> smoothingFilterType;
    smoothingFilterType::Pointer smoohingFilter = smoothingFilterType::New();
    smoohingFilter->SetSigma(smoothSigma);
    smoohingFilter->SetInput(itkImage);
    smoohingFilter->Update();
    itkImage = smoohingFilter->GetOutput();
  }

  //Thresholding
  ssc::messageManager()->sendDebug("Thresholding...");
  typedef itk::BinaryThresholdImageFilter<itkImageType, itkImageType> thresholdFilterType;
  thresholdFilterType::Pointer thresholdFilter = thresholdFilterType::New();
  thresholdFilter->SetInput(itkImage);
  //TODO:  support non-binary images
  thresholdFilter->SetOutsideValue(0);
  thresholdFilter->SetInsideValue(1);
  thresholdFilter->SetLowerThreshold(threshold);
  thresholdFilter->Update();
  itkImage = thresholdFilter->GetOutput();

  //Convert ITK to VTK
  itkToVtkFilterType::Pointer itkToVtkFilter = itkToVtkFilterType::New();
  itkToVtkFilter->SetInput(itkImage);
  itkToVtkFilter->Update();

  vtkImageDataPtr rawResult = vtkImageDataPtr::New();
  //itkToVtkFilter->GetOutput()->ReleaseDataFlagOn();// Test: see if this release more memory: No change here
  rawResult->DeepCopy(itkToVtkFilter->GetOutput());
  // TODO: possible memory problem here - check debug mem system of itk/vtk


  QString uid = ssc::changeExtension(image->getUid(), "") + "_seg%1";
  QString name = image->getName()+" segmented %1";
  //std::cout << "segmented volume: " << uid << ", " << name << std::endl;
  ssc::ImagePtr result = ssc::dataManager()->createImage(rawResult,uid, name);
  ssc::messageManager()->sendInfo("created segment " + result->getName());

  result->get_rMd_History()->setRegistration(image->get_rMd());

  result->setParentFrame(image->getUid());
  ssc::dataManager()->loadData(result);
  ssc::dataManager()->saveImage(result, outputBasePath);

  return result;
}

ssc::ImagePtr Segmentation::centerline(ssc::ImagePtr image, QString outputBasePath)
{
  ssc::messageManager()->sendInfo("Finding "+image->getName()+"s centerline... Please wait!");

  QDateTime startTime = QDateTime::currentDateTime();

  itkImageType::ConstPointer itkImage = getITKfromSSCImage(image);

  //Centerline extraction
  typedef itk::BinaryThinningImageFilter3D<itkImageType, itkImageType> centerlineFilterType;
  centerlineFilterType::Pointer centerlineFilter = centerlineFilterType::New();
  centerlineFilter->SetInput(itkImage);
  centerlineFilter->Update();
  itkImage = centerlineFilter->GetOutput();

  //Convert ITK to VTK
  itkToVtkFilterType::Pointer itkToVtkFilter = itkToVtkFilterType::New();
  itkToVtkFilter->SetInput(itkImage);
  itkToVtkFilter->Update();

  vtkImageDataPtr rawResult = vtkImageDataPtr::New();
  rawResult->DeepCopy(itkToVtkFilter->GetOutput());

  QString uid = ssc::changeExtension(image->getUid(), "") + "_center%1";
  QString name = image->getName()+" centerline %1";
  //std::cout << "segmented volume: " << uid << ", " << name << std::endl;
  ssc::ImagePtr result = ssc::dataManager()->createImage(rawResult,uid, name);
  ssc::messageManager()->sendInfo("created centerline " + result->getName());

  result->get_rMd_History()->setRegistration(image->get_rMd());

  QTime tempTime = QTime(0, 0);
  tempTime = tempTime.addMSecs(startTime.time().msecsTo(QDateTime::currentDateTime().time()));
  ssc::messageManager()->sendInfo("Generating centerline time: " + tempTime.toString("hh:mm:ss:zzz"));

  result->setParentFrame(image->getUid());
  ssc::dataManager()->loadData(result);
  ssc::dataManager()->saveImage(result, outputBasePath);

  return result;
}



/** Crop the image to the bounding box bb_q.
 *  bb_q is given in the output space q, defined relative to the image space d
 *  with qMd. If qMd is non-identity, image is resampled to space q.
 *  outputSpacing can be used to resample the volume (after cropping).
 */
ssc::ImagePtr Segmentation::resample(ssc::ImagePtr image, ssc::ImagePtr reference, QString outputBasePath, double margin)
{
  if (!image || !reference)
    return ssc::ImagePtr();

  ssc::Transform3D refMi = reference->get_rMd().inv() * image->get_rMd();
  ssc::ImagePtr oriented = resampleImage(image, refMi);

  ssc::Transform3D orient_M_ref = oriented->get_rMd().inv() * reference->get_rMd();
  ssc::DoubleBoundingBox3D bb_crop = transform(orient_M_ref, reference->boundingBox());

  // increase bb size by margin
  bb_crop[0] -= margin;
  bb_crop[1] += margin;
  bb_crop[2] -= margin;
  bb_crop[3] += margin;
  bb_crop[4] -= margin;
  bb_crop[5] += margin;

  oriented->setCroppingBox(bb_crop);

//  ssc::dataManager()->loadData(oriented);
//  ssc::dataManager()->saveImage(oriented, outputBasePath);

  ssc::ImagePtr cropped = cropImage(oriented);
//  dataManager()->loadData(cropped);
//  dataManager()->saveImage(cropped, outputBasePath);

  ssc::ImagePtr resampled = resampleImage(cropped, ssc::Vector3D(reference->getBaseVtkImageData()->GetSpacing()));
  ssc::dataManager()->loadData(resampled);
  ssc::dataManager()->saveImage(resampled, outputBasePath);

  resampled->getBaseVtkImageData()->Print(std::cout);
  return resampled;
}

} // namespace cx


