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
#include "vtkForwardDeclarations.h"
#include "sscTypeConversions.h"
#include "sscRegistrationTransform.h"
#include "sscImage.h"
#include "sscDataManager.h"
#include "sscUtilHelpers.h"
#include "sscMesh.h"
#include "sscMessageManager.h"
#include "vtkForwardDeclarations.h"

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

/*void Segmentation::tubeContour(ssc::ImagePtr image, QString outputBasePath)
{
  //Don't work as vtkTubeFilter need a vtkPolyData as input
  typedef vtkSmartPointer<class vtkTubeFilter> vtkTubeFilterPtr;
  vtkTubeFilterPtr tube = vtkTubeFilterPtr::New();
  tube->SetInput(image->getBaseVtkImageData());
  tube->SetRadius(1.0);
  tube->SetNumberOfSides(12);
  tube->Update();
  vtkPolyDataPtr tubePolyData = vtkPolyDataPtr::New();
  tubePolyData = tube->GetOutput();


  QString uid = ssc::changeExtension(image->getUid(), "") + "_tube%1";
  QString name = image->getName() + " tubes %1";
  //std::cout << "contoured volume: " << uid << ", " << name << std::endl;
  ssc::MeshPtr result = ssc::dataManager()->createMesh(tubePolyData, uid,
      name, "Images");
  ssc::messageManager()->sendInfo("created tubes " + result->getName());

  result->get_rMd_History()->setRegistration(image->get_rMd());
  result->setParentFrame(image->getUid());

  ssc::dataManager()->loadData(result);
  ssc::dataManager()->saveMesh(result, outputBasePath);
}*/
/*{
  //Create vtkPolyData
  vtkMarchingCubesPtr convert = vtkMarchingCubesPtr::New();
  convert->SetInput(rawResult);
  convert->Update();
  vtkPolyDataPtr cubesPolyData = vtkPolyDataPtr::New();
  cubesPolyData = convert->GetOutput();

  //Show surface
  ssc::MeshPtr surface = ssc::MeshPtr(new ssc::Mesh(outName.toStdString()+"_segm"));
  surface->setVtkPolyData(cubesPolyData);
}*/

} // namespace cx


//void ShiftCorrectionWidget::segmentImage(QString imageName,
//                                         int thresholdValue,
//                                         bool smoothing)
//{
//  QString filePath = mWorkingFolder+"/"+imageName;
//
//  //Read image file
//  //ITK
//  ImageReaderType::Pointer  reader  = ImageReaderType::New();
//  reader->SetFileName(filePath.toLatin1());
//  reader->Update();
//
//  ImageDirectionType direction = reader->GetOutput()->GetDirection();
//  //std::cout << "Matrix = " << std::endl << direction << std::endl;
//  int i,j;
//  vtkMatrix4x4 *matrix = vtkMatrix4x4::New();
//  for (i=0;i<3;i++)
//  {
//    for(j=0;j<3;j++)
//    {
//      matrix->SetElement(i, j, direction.GetVnlMatrix()[i][j]);
//      //std::cout << "Matrix element = " << matrix->GetElement(i,j) << std::endl;
//    }
//  }
//
//  ImageType::Pointer data = reader->GetOutput();
//
//  //Smooting
//  if(smoothing)
//  {
//    typedef itk::SmoothingRecursiveGaussianImageFilter<ImageType, ImageType> smoothingFilterType;
//    smoothingFilterType::Pointer smoohingFilter = smoothingFilterType::New();
//    smoohingFilter->SetSigma(0.5);
//    smoohingFilter->SetInput(data);
//    smoohingFilter->Update();
//    data = smoohingFilter->GetOutput();
//  }
//
//  //Thresholding
//  typedef itk::BinaryThresholdImageFilter<ImageType, ImageType> thresholdFilterType;
//  thresholdFilterType::Pointer thresholdFilter = thresholdFilterType::New();
//  thresholdFilter->SetInput(data);
//  thresholdFilter->SetOutsideValue(0);
//  thresholdFilter->SetInsideValue(1);
//  thresholdFilter->SetLowerThreshold(thresholdValue);
//  thresholdFilter->Update();
//  data = thresholdFilter->GetOutput();
//
//  //Test writer
//  QString outName = filePath;
//  if(outName.endsWith(".mhd"))
//    outName.replace(QString(".mhd"), QString(""));
//  if(outName.endsWith(".mha"))
//    outName.replace(QString(".mha"), QString(""));
//
//  QString outFileName = outName+"_segm_converted.mhd";
//  QString rawFileName = outName+"_segm_converted.raw";
//
//
//  //Convert ITK to VTK
//  itkToVtkFilterType::Pointer itkToVtkFilter = itkToVtkFilterType::New();
//  //itkToVtkFilter->SetInput(data);
//  itkToVtkFilter->SetInput(thresholdFilter->GetOutput());
//  itkToVtkFilter->Update();
//
//  //Get 3D view
//  View3D* view = ViewManager::getInstance()->get3DView();
//
//  //Show converted volume = empty?
//  /*ssc::ImagePtr image = ssc::ImagePtr(new ssc::Image(outName.toStdString()+"_segm_volume",
//                                                     itkToVtkFilter->GetOutput()));
//  ssc::VolumetricRepPtr volumetricRep(ssc::VolumetricRep::New(outName.toStdString()+"_segm_volume"));
//  volumetricRep->setImage(image);
//
//  //Crash???
//  view->setRep(volumetricRep);*/
//
//
//  //test Save vtk object
//  /*typedef vtkSmartPointer<vtkMetaImageWriter> vtkMetaImageWriterPtr;
//  vtkMetaImageWriterPtr vtkWriter = vtkMetaImageWriterPtr::New();
//  vtkWriter->SetInput(itkToVtkFilter->GetOutput());
//  vtkWriter->SetFileName( outFileName.toLatin1() );
//  vtkWriter->SetRAWFileName( rawFileName.toLatin1() );
//  vtkWriter->SetCompression(false);
//  vtkWriter->Update();
//  vtkWriter->Write();*/
//
//
//  outFileName = outName+"_segm.mhd";
//
//  //Create vtkPolyData
//  /*vtkImageToPolyDataFilter* convert = vtkImageToPolyDataFilter::New();
//  convert->SetInput(itkToVtkFilter->GetOutput());
//  convert->SetColorModeToLinear256();
//  convert->Update();*/
//
//  vtkMarchingCubesPtr convert = vtkMarchingCubesPtr::New();
//  convert->SetInput(itkToVtkFilter->GetOutput());
//  //convert->SetValue(0, 150);
//  convert->Update();
//  //messageManager()->sendInfo("Number of contours: "+QString::number(convert->GetNumberOfContours()).toStdString());
//
//  vtkPolyDataPtr cubesPolyData = vtkPolyDataPtr::New();
//  cubesPolyData = convert->GetOutput();
//
//  //print
//  //itkToVtkFilter->GetOutput()->Print(std::cout);
//  cubesPolyData->Print(std::cout);
//  //vtkPolyData* cubesPolyData = convert->GetOutput();
//
//  ssc::MeshPtr surface = ssc::MeshPtr(new ssc::Mesh(outName.toStdString()+"_segm"));
//  surface->setVtkPolyData(cubesPolyData);
//  ssc::GeometricRepPtr surfaceRep(ssc::GeometricRep::New(outName.toStdString()+"_segm"));
//  surfaceRep->setMesh(surface);
//
//  view->addRep(surfaceRep);
//
//  //Cone test
//  typedef vtkSmartPointer<vtkConeSource> vtkConeSourcePtr;
//  vtkConeSourcePtr coneSource = vtkConeSource::New();
//  coneSource->SetResolution(25);
//  coneSource->SetRadius(10);
//  coneSource->SetHeight(100);
//
//  coneSource->SetDirection(0,0,1);
//  double newCenter[3];
//  coneSource->GetCenter(newCenter);
//  newCenter[2] = newCenter[2] - coneSource->GetHeight()/2;
//  coneSource->SetCenter(newCenter);
//
//  //Cone rep visialization
//  ssc::MeshPtr coneSurface = ssc::MeshPtr(new ssc::Mesh("cone"));
//  coneSurface->setVtkPolyData(coneSource->GetOutput());
//  ssc::GeometricRepPtr coneSurfaceRep(ssc::GeometricRep::New("cone"));
//  coneSurfaceRep->setMesh(coneSurface);
//  view->addRep(coneSurfaceRep);
//
//  //print
//  coneSource->Update();
//  coneSource->GetOutput()->Print(std::cout);
//  //vtkPolyData* conePolyData = coneSource->GetOutput();
//
//  //vtkPolyData* surface = convert->GetOutput();
//
//  //test: Show surface
//  /*vtkPolyDataMapper* mapper = vtkPolyDataMapper::New();
//  mapper->SetInput(surface);
//  mapper->Update();
//  vtkActor* actor = vtkActor::New();
//  actor->SetMapper(mapper);
//  actor->GetProperty()->SetColor(1.0, 0.0, 0.0);
//  actor->SetVisibility(true);
//  actor->SetUserMatrix(matrix);
//
//  ViewManager::getInstance()->get3DView("View3D_1")->getRenderer()->AddActor(actor);
//  */
//
//  //Test save ITK object
//  ImageWriterType::Pointer writer = ImageWriterType::New();
//  writer->SetInput(data);
//  writer->SetFileName( outFileName.toLatin1() );
//  writer->Update();
//}
