/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxThreadedTimedAlgorithm.h"

#include "cxTimedAlgorithm.h"

#include "itkImageFileReader.h"
#include "cxTypeConversions.h"
#include "vtkMetaImageWriter.h"
#include "cxSettings.h"
#include "cxUtilHelpers.h"

namespace cx
{



template class ThreadedTimedAlgorithm<vtkImageDataPtr>; //centerline
template class ThreadedTimedAlgorithm<ImagePtr>; //resample
//template class ThreadedTimedAlgorithm<vtkImageDataPtr>; //BinaryThresholdImageFilter
template class ThreadedTimedAlgorithm<vtkPolyDataPtr>; //contour
template class ThreadedTimedAlgorithm<void>;
template class ThreadedTimedAlgorithm<bool>;

template<>
cxResource_EXPORT void ThreadedTimedAlgorithm<void>::getResult()
{
	return;
}


//---------------------------------------------------------------------------------------------------------------------
Example::Example() :
    ThreadedTimedAlgorithm<QString>("TestProduct", 1)
{
  std::cout << "Test::Test()" << std::endl;
  this->generate();
}

Example::~Example()
{}

void Example::postProcessingSlot()
{
  QString result = this->getResult();
  std::cout << "void Test::postProcessingSlot(), result: "<< result.toStdString() << std::endl;
  emit finished();
}

QString Example::calculate()
{
  std::cout << " QString Test::calculate()" << std::endl;

#ifdef WIN32
  Sleep(500);
#else
  sleep(5);
#endif

  return QString("Test successful!!!");
}
}//namespace cx
