/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#include "cxAlgorithmHelpers.h"

#include "cxImage.h"
#include "cxTypeConversions.h"
#include "itkImageFileReader.h"
#include "vtkMetaImageWriter.h"
#include "cxSettings.h"
#include <QDir>
#include "cxUtilHelpers.h"
#include "cxDataLocations.h"
#include "cxLogger.h"
#include <itkGrayscaleFillholeImageFilter.h>

namespace cx
{

//---------------------------------------------------------------------------------------------------------------------
itkImageType::ConstPointer AlgorithmHelper::getITKfromVTKImage(vtkImageDataPtr image)
{
  //HACK
  return AlgorithmHelper::getITKfromVTKImageViaFile(image);
}
//---------------------------------------------------------------------------------------------------------------------

itkImageType::ConstPointer AlgorithmHelper::getITKfromSSCImage(ImagePtr input)
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
  	reportWarning("Image values out of range. max: " + qstring_cast(maxVal)
  			+ " min: " + qstring_cast(minVal) + " See bug #363 if this needs to be fixed");

  QString tempFolder = DataLocations::getCachePath() + "/vtk2itk/";
  QDir().mkpath(tempFolder);

  // write to disk
  vtkMetaImageWriterPtr writer = vtkMetaImageWriterPtr::New();

  QString filename = tempFolder + "/"+qstring_cast(writer.GetPointer())+".mhd";


  writer->SetInputData(input);
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
  QFile(changeExtension(filename, "raw")).remove(); // cleanup

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
