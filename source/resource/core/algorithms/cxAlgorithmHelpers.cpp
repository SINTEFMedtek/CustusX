/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors 
   may be used to endorse or promote products derived from this software 
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
