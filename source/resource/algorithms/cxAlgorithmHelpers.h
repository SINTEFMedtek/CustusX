/*
 * cxAlgorothmHelpers.h
 *
 *  Created on: Jun 27, 2012
 *      Author: christiana
 */

#ifndef CXALGORITHMHELPERS_H_
#define CXALGORITHMHELPERS_H_

#include <itkSmoothingRecursiveGaussianImageFilter.h>
#include <itkBinaryThresholdImageFilter.h>
#include <itkBinaryThinningImageFilter3D.h>
#include "ItkVtkGlue/itkImageToVTKImageFilter.h"
#include "ItkVtkGlue/itkVTKImageToImageFilter.h"
#include <itkImage.h>

#include <vtkImageCast.h>
#include "sscForwardDeclarations.h"

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

private:
  static itkImageType::ConstPointer getITKfromSSCImageViaFile(ssc::ImagePtr image);
};


}

#endif /* CXALGOROTHMHELPERS_H_ */
