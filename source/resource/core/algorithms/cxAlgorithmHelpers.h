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

#ifndef CXALGORITHMHELPERS_H_
#define CXALGORITHMHELPERS_H_

#include "cxResourceExport.h"

#include "ItkVtkGlue/itkImageToVTKImageFilter.h"
#include "ItkVtkGlue/itkVTKImageToImageFilter.h"
#include <itkImage.h>

#include "cxForwardDeclarations.h"
#include "vtkForwardDeclarations.h"

namespace cx
{

const unsigned int Dimension = 3;
//typedef unsigned short PixelType;
typedef short PixelType; //short will probably work in most cases, but int may be needed
typedef itk::Image< PixelType, Dimension >  itkImageType;
typedef itk::ImageToVTKImageFilter<itkImageType> itkToVtkFilterType;
typedef itk::VTKImageToImageFilter<itkImageType> itkVTKImageToImageFilterType;


/**
 * \brief Class with helper functions for algorithms.
 * \ingroup cx_resource_core_algorithms
 *
 * \date Feb 16, 2011
 * \author Janne Beate Bakeng, SINTEF
 */
class cxResource_EXPORT AlgorithmHelper
{
public:
  static itkImageType::ConstPointer getITKfromSSCImage(ImagePtr image);
  static itkImageType::ConstPointer getITKfromVTKImage(vtkImageDataPtr image);

  static vtkImageDataPtr getVTKFromITK(itkImageType::ConstPointer input);
  static vtkImageDataPtr execute_itk_GrayscaleFillholeImageFilter(vtkImageDataPtr input);

private:
  static itkImageType::ConstPointer getITKfromVTKImageViaFile(vtkImageDataPtr image);
};


}

#endif /* CXALGOROTHMHELPERS_H_ */
