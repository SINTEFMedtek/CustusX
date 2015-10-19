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

#include "cxConnectedThresholdImageFilter.h"

#include "itkConnectedThresholdImageFilter.h"
#include "cxLogger.h"
#include "cxTypeConversions.h"
#include "cxAlgorithmHelpers.h"
#include "cxRegistrationTransform.h"
#include "cxImage.h"
#include "cxPatientModelService.h"
#include "cxVolumeHelpers.h"
#include "cxVisServices.h"

namespace cx
{

ConnectedThresholdImageFilter::ConnectedThresholdImageFilter(VisServicesPtr services) :
	ThreadedTimedAlgorithm<vtkImageDataPtr>("segmenting", 10),
	mServices(services)
{
}

ConnectedThresholdImageFilter::~ConnectedThresholdImageFilter()
{
}

void ConnectedThresholdImageFilter::setInput(ImagePtr image, QString outputBasePath, float lowerThreshold, float upperThreshold, int replaceValue, itkImageType::IndexType seed)
{
	mInput = image;
	mOutputBasePath = outputBasePath;

	mLowerThreshold = lowerThreshold;
	mUpperTheshold = upperThreshold;
	mReplaceValue = replaceValue;
	mSeed = seed;

	this->generate();
}

ImagePtr ConnectedThresholdImageFilter::getOutput()
{
	return mOutput;
}

void ConnectedThresholdImageFilter::postProcessingSlot()
{
	//get the result from the thread
	vtkImageDataPtr rawResult = this->getResult();

	if(!rawResult)
	{
		reportError("Segmentation failed.");
		return;
	}

	//generate a new name and unique id for the newly created object
	QString uid = mInput->getUid() + "_seg%1";
	QString name = mInput->getName()+" seg%1";

	//create a Image
	ImagePtr result = createDerivedImage(mServices->patient(),
										 uid, name,
										 rawResult, mInput);
	mOutput->resetTransferFunctions();

	mServices->patient()->insertData(mOutput);

	//let the user know you are finished
	reportSuccess("Done segmenting: \"" + mOutput->getName()+"\"");

	//let the system know you're finished
	//  emit finished();
}

vtkImageDataPtr ConnectedThresholdImageFilter::calculate()
{
	//Connected Thresholding

	//Documentation:
	// http://www.na-mic.org/svn/Slicer3-lib-mirrors/trunk/Insight/Examples/Segmentation/ConnectedThresholdImageFilter.cxx

	//  The ConnectedThresholdImageFilter has two main parameters to be
	//  defined. They are the lower and upper thresholds of the interval in
	//  which intensity values should fall in order to be included in the
	//  region. Setting these two values too close will not allow enough
	//  flexibility for the region to grow. Setting them too far apart will
	//  result in a region that engulfs the image.

	//  The output of this filter is a binary image with zero-value pixels
	//  everywhere except on the extracted region. The intensity value set
	//  inside the region is selected with the method SetReplaceValue()

	//  The initialization of the algorithm requires the user to provide a seed
	//  point. It is convenient to select this point to be placed in a
	//  typical region of the anatomical structure to be segmented. The
	//  seed is passed in the form of a IndexType to the SetSeed()
	//  method.

	//  Another option for segmenting regions is to take advantage of the
	//  functionality provided by the ConnectedThresholdImageFilter for
	//  managing multiple seeds. The seeds can be passed one by one to the
	//  filter using the AddSeed() method. You could imagine a user
	//  interface in which an operator clicks on multiple points of the object
	//  to be segmented and each selected point is passed as a seed to this
	//  filter.

	itkImageType::ConstPointer itkImage = AlgorithmHelper::getITKfromSSCImage(mInput);

	typedef itk::ConnectedThresholdImageFilter<itkImageType, itkImageType> thresholdFilterType;
	thresholdFilterType::Pointer thresholdFilter = thresholdFilterType::New();
	thresholdFilter->SetInput(itkImage);

	//set thresholds
	thresholdFilter->SetLower(mLowerThreshold);
	thresholdFilter->SetUpper(mUpperTheshold);
	thresholdFilter->SetReplaceValue(mReplaceValue);

	//set seeds
	thresholdFilter->SetSeed(mSeed);

	//calculate
	try
	{
		thresholdFilter->Update();
	}
	catch( itk::ExceptionObject & excep )
	{
		reportError("Error when setting seed for Connected Threshold Image Filter:");
		reportError(qstring_cast(excep.GetDescription()));
	}

	itkImage = thresholdFilter->GetOutput();

	//Convert ITK to VTK
	itkToVtkFilterType::Pointer itkToVtkFilter = itkToVtkFilterType::New();
	itkToVtkFilter->SetInput(itkImage);
	itkToVtkFilter->Update();

	vtkImageDataPtr rawResult = vtkImageDataPtr::New();
	rawResult->DeepCopy(itkToVtkFilter->GetOutput());
	// TODO: possible memory problem here - check debug mem system of itk/vtk

	return rawResult;
}

}
