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

#include <itkGrayscaleFillholeImageFilter.h>

namespace cx
{

//---------------------------------------------------------------------------------------------------------------------
itkImageType::ConstPointer AlgorithmHelper::getITKfromVTKImage(vtkImageDataPtr image)
{
  //HACK
  return AlgorithmHelper::getITKfromVTKImageViaFile(image);

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

itkImageType::ConstPointer AlgorithmHelper::getITKfromSSCImage(ssc::ImagePtr input)
{
	if (!input)
		return getITKfromVTKImage(vtkImageDataPtr());
	else
		return getITKfromVTKImage(input->getBaseVtkImageData());
}
//---------------------------------------------------------------------------------------------------------------------

/**This is a workaround for _unpredictable_ crashes
 * experienced when using the itk::VTKImageToImageFilter.
 *
 */
itkImageType::ConstPointer AlgorithmHelper::getITKfromVTKImageViaFile(vtkImageDataPtr input)
{
  if(!input)
  {
    std::cout << "getITKfromSSCImage(): NO image!!!" << std::endl;
    return itkImageType::ConstPointer();
  }

  double minVal = input->GetScalarRange()[0];
  double maxVal = input->GetScalarRange()[1];

  if(maxVal > SHRT_MAX || minVal < SHRT_MIN)
  	ssc::messageManager()->sendWarning("Image values out of range. max: " + qstring_cast(maxVal)
  			+ " min: " + qstring_cast(minVal) + " See bug #363 if this needs to be fixed");

  QString tempFolder = settings()->value("globalPatientDataFolder").toString() + "/NoPatient/temp/";
  QDir().mkpath(tempFolder);

  // write to disk
  vtkMetaImageWriterPtr writer = vtkMetaImageWriterPtr::New();

  QString filename = tempFolder + "/"+qstring_cast(writer.GetPointer())+".mhd";


  writer->SetInput(input);
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

vtkImageDataPtr AlgorithmHelper::getVTKFromITK(itkImageType::ConstPointer input)
{
	//Convert ITK to VTK
	itkToVtkFilterType::Pointer itkToVtkFilter = itkToVtkFilterType::New();
	itkToVtkFilter->SetInput(input);
	itkToVtkFilter->Update();

	vtkImageDataPtr rawResult = vtkImageDataPtr::New();
	rawResult->DeepCopy(itkToVtkFilter->GetOutput());
	// TODO: possible memory problem here - check debug mem system of itk/vtk

	return rawResult;
}

vtkImageDataPtr AlgorithmHelper::execute_itk_GrayscaleFillholeImageFilter(vtkImageDataPtr input)
{
  itkImageType::ConstPointer itkImage = AlgorithmHelper::getITKfromVTKImage(input);

  typedef itk::GrayscaleFillholeImageFilter<itkImageType, itkImageType> FilterType;
  FilterType::Pointer filter = FilterType::New();

  filter->SetInput(itkImage);
  filter->Update();

  return AlgorithmHelper::getVTKFromITK(filter->GetOutput());
}


}
