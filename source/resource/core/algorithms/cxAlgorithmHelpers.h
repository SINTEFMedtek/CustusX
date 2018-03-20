/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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
