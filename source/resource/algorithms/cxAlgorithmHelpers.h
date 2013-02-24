// This file is part of CustusX, an Image Guided Therapy Application.
//
// Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
//
// CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
// code and binaries can only be used by SMT and those with explicit permission
// from SMT. CustusX shall not be distributed to anyone else.
//
// CustusX is a research tool. It is NOT intended for use or certified for use
// in a normal clinical setting. SMT does not take responsibility for its use
// in any way.
//
// See CustusX_License.txt for more information.

#ifndef CXALGORITHMHELPERS_H_
#define CXALGORITHMHELPERS_H_

#include "ItkVtkGlue/itkImageToVTKImageFilter.h"
#include "ItkVtkGlue/itkVTKImageToImageFilter.h"
#include <itkImage.h>

#include "sscForwardDeclarations.h"
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
 * \ingroup cxResourceAlgorithms
 *
 * \date Feb 16, 2011
 * \author Janne Beate Bakeng, SINTEF
 */
class AlgorithmHelper
{
public:
  static itkImageType::ConstPointer getITKfromSSCImage(ssc::ImagePtr image);
  static itkImageType::ConstPointer getITKfromVTKImage(vtkImageDataPtr image);

  static vtkImageDataPtr getVTKFromITK(itkImageType::ConstPointer input);
  static vtkImageDataPtr execute_itk_GrayscaleFillholeImageFilter(vtkImageDataPtr input);

private:
  static itkImageType::ConstPointer getITKfromVTKImageViaFile(vtkImageDataPtr image);
};


}

#endif /* CXALGOROTHMHELPERS_H_ */
