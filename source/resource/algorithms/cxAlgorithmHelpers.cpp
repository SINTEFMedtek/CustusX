/*
 * cxAlgorothmHelpers.cpp
 *
 *  Created on: Jun 27, 2012
 *      Author: christiana
 */

#include <cxAlgorithmHelpers.h>
#include "sscMessageManager.h"
#include "sscImage.h"
#include "sscTypeConversions.h"
#include "itkImageFileReader.h"
#include "vtkMetaImageWriter.h"
#include "cxSettings.h"
#include <QDir>
#include "sscUtilHelpers.h"

namespace cx
{

//---------------------------------------------------------------------------------------------------------------------
itkImageType::ConstPointer AlgorithmHelper::getITKfromSSCImage(ssc::ImagePtr image)
{
  //HACK
  return AlgorithmHelper::getITKfromSSCImageViaFile(image);

//  if(!image)
//  {
//    std::cout << "getITKfromSSCImage(): NO image!!!" << std::endl;
//    return itkImageType::ConstPointer();
//  }
//  itkVTKImageToImageFilterType::Pointer vtk2itkFilter = itkVTKImageToImageFilterType::New();
//  //itkToVtkFilter->SetInput(data);
//  vtkImageDataPtr input = image->getBaseVtkImageData();
//  if (input->GetScalarType() != VTK_UNSIGNED_SHORT)
//  //if (input->GetScalarType() == VTK_UNSIGNED_CHAR)
//  {
//    // convert
//    // May need to use vtkImageShiftScale instead if we got data types other than unsigned char?
//    vtkImageCastPtr imageCast = vtkImageCastPtr::New();
//    imageCast->SetInput(input);
//    imageCast->SetOutputScalarTypeToUnsignedShort();
//    input = imageCast->GetOutput();
//  }
//  vtk2itkFilter->SetInput(input);
//  vtk2itkFilter->Update();
//  return vtk2itkFilter->GetOutput();
}
//---------------------------------------------------------------------------------------------------------------------


/**This is a workaround for _unpredictable_ crashes
 * experienced when using the itk::VTKImageToImageFilter.
 *
 */
itkImageType::ConstPointer AlgorithmHelper::getITKfromSSCImageViaFile(ssc::ImagePtr image)
{
  if(!image)
  {
    std::cout << "getITKfromSSCImage(): NO image!!!" << std::endl;
    return itkImageType::ConstPointer();
  }

  if(image->getMax() > SHRT_MAX || image->getMin() < SHRT_MIN)
  	ssc::messageManager()->sendWarning("Image values out of range. max: " + qstring_cast(image->getMax())
  			+ " min: " + qstring_cast(image->getMin()) + " See bug #363 if this needs to be fixed");

  QString tempFolder = settings()->value("globalPatientDataFolder").toString() + "/NoPatient/temp/";
  QDir().mkpath(tempFolder);

  // write to disk
  vtkMetaImageWriterPtr writer = vtkMetaImageWriterPtr::New();

  QString filename = tempFolder + "/"+qstring_cast(writer.GetPointer())+".mhd";


  writer->SetInput(image->getBaseVtkImageData());
  writer->SetFileName(cstring_cast(filename));
  writer->SetCompression(false);
  writer->Write();

  // read from disk
  typedef itk::ImageFileReader<itkImageType> ReaderType;
  ReaderType::Pointer reader = ReaderType::New();
#if ITK_VERSION_MAJOR==3
  reader->SetFileName(cstring_cast(filename));
#else
  reader->SetFileName(string_cast(filename));
#endif

////  reader->SetFileName(cstring_cast(filename));
//  reader->SetFileName(string_cast(filename));
  reader->Update();
  itkImageType::ConstPointer retval = reader->GetOutput();

  QFile(filename).remove(); // cleanup
  QFile(ssc::changeExtension(filename, "raw")).remove(); // cleanup

  return retval;
}
//---------------------------------------------------------------------------------------------------------------------


}
