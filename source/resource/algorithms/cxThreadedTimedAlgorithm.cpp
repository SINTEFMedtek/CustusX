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

#include "cxThreadedTimedAlgorithm.h"

#include "cxTimedAlgorithm.h"

#include "itkImageFileReader.h"
#include "sscTypeConversions.h"
#include "vtkMetaImageWriter.h"
#include "cxSettings.h"
#include "sscUtilHelpers.h"

namespace cx
{



template class ThreadedTimedAlgorithm<vtkImageDataPtr>; //centerline
template class ThreadedTimedAlgorithm<ssc::ImagePtr>; //resample
//template class ThreadedTimedAlgorithm<vtkImageDataPtr>; //BinaryThresholdImageFilter
template class ThreadedTimedAlgorithm<vtkPolyDataPtr>; //contour


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
