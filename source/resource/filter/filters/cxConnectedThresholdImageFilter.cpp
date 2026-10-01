/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxConnectedThresholdImageFilter.h"

#include <vtkImageData.h>
#include <vtkPoints.h>
#include <vtkImageCast.h>
#include <vtkImageThresholdConnectivity.h>
#include "cxLogger.h"
#include "cxTypeConversions.h"
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

void ConnectedThresholdImageFilter::setInput(ImagePtr image, QString outputBasePath, float lowerThreshold, float upperThreshold, int replaceValue, Eigen::Array3i seed)
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
	mOutput = createDerivedImage(mServices->patient(),
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
	vtkImageDataPtr retval = segment(mInput->getBaseVtkImageData(), mLowerThreshold, mUpperTheshold, mReplaceValue, mSeed);
	if (!retval)
	{
		reportError(QString("Connected Threshold Image Filter: seed (%1, %2, %3) is outside the image.")
					.arg(mSeed[0]).arg(mSeed[1]).arg(mSeed[2]));
	}
	return retval;
}

vtkImageDataPtr ConnectedThresholdImageFilter::segment(vtkImageDataPtr image, double lower, double upper, int replaceValue, Eigen::Array3i seed)
{
	vtkImageDataPtr retval;
	const int* dims = image->GetDimensions();
	const bool seedInside = (seed >= 0).all() && seed[0] < dims[0] && seed[1] < dims[1] && seed[2] < dims[2];
	if (seedInside)
	{
		const int* extent = image->GetExtent();
		const double* origin = image->GetOrigin();
		const double* spacing = image->GetSpacing();
		vtkSmartPointer<vtkPoints> seedPoints = vtkSmartPointer<vtkPoints>::New();
		seedPoints->InsertNextPoint(origin[0] + (extent[0]+seed[0])*spacing[0],
									origin[1] + (extent[2]+seed[1])*spacing[1],
									origin[2] + (extent[4]+seed[2])*spacing[2]);

		vtkImageCastPtr cast = vtkImageCastPtr::New();
		cast->SetInputData(image);
		cast->SetOutputScalarTypeToShort();

		vtkSmartPointer<vtkImageThresholdConnectivity> connectivity = vtkSmartPointer<vtkImageThresholdConnectivity>::New();
		connectivity->SetInputConnection(cast->GetOutputPort());
		connectivity->SetSeedPoints(seedPoints);
		connectivity->ThresholdBetween(lower, upper);
		connectivity->SetInValue(replaceValue);
		connectivity->SetOutValue(0);
		connectivity->ReplaceInOn();
		connectivity->ReplaceOutOn();
		connectivity->Update();
		retval = connectivity->GetOutput();
	}
	return retval;
}

}
