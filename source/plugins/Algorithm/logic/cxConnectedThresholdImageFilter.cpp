#include "cxConnectedThresholdImageFilter.h"

#include "itkConnectedThresholdImageFilter.h"
#include "sscMessageManager.h"
#include "sscTypeConversions.h"
#include "cxAlgorithmHelpers.h"

#include "sscDataManager.h"
#include "sscRegistrationTransform.h"


namespace cx
{

ConnectedThresholdImageFilter::ConnectedThresholdImageFilter() :
    ThreadedTimedAlgorithm<vtkImageDataPtr>("segmenting", 10)
{
}

ConnectedThresholdImageFilter::~ConnectedThresholdImageFilter()
{
}

void ConnectedThresholdImageFilter::setInput(ssc::ImagePtr image, QString outputBasePath, float lowerThreshold, float upperThreshold, int replaceValue, itkImageType::IndexType seed)
{
	mInput = image;
	mOutputBasePath = outputBasePath;

	mLowerThreshold = lowerThreshold;
	mUpperTheshold = upperThreshold;
	mReplaceValue = replaceValue;
	mSeed = seed;

	this->generate();
}

ssc::ImagePtr ConnectedThresholdImageFilter::getOutput()
{
	return mOutput;
}

void ConnectedThresholdImageFilter::postProcessingSlot()
{
	//get the result from the thread
	vtkImageDataPtr rawResult = this->getResult();

	//generate a new name and unique id for the newly created object
	QString uid = mInput->getUid() + "_seg%1";
	QString name = mInput->getName()+" seg%1";

	//create a ssc::Image
	mOutput = ssc::dataManager()->createDerivedImage(rawResult,uid, name, mInput);
	if(!mOutput)
	{
		ssc::messageManager()->sendError("Segmentation failed.");
		return;
	}
	mOutput->resetTransferFunctions();

	//load the image into CustusX for visualization
	ssc::dataManager()->loadData(mOutput);

	//save the data to file
	ssc::dataManager()->saveImage(mOutput, mOutputBasePath);

	//let the user know you are finished
	ssc::messageManager()->sendSuccess("Done segmenting: \"" + mOutput->getName()+"\"");

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
		ssc::messageManager()->sendError("Error when setting seed for Connected Threshold Image Filter:");
		ssc::messageManager()->sendError(qstring_cast(excep.GetDescription()), true);
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
