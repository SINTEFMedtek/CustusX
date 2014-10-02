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
template class cxResource_EXPORT ThreadedTimedAlgorithm<void>;

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
