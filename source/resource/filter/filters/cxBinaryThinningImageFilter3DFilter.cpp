/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxBinaryThinningImageFilter3DFilter.h"

#include <itkBinaryThinningImageFilter3D.h>
#include <itkBinaryThresholdImageFilter.h>

#include "cxLogger.h"
#include "cxRegistrationTransform.h"
#include "cxMesh.h"
#include "cxImage.h"
#include "cxColorProperty.h"
#include "vesselReg/SeansVesselReg.hxx"
#include "cxSelectDataStringProperty.h"
#include "cxAlgorithmHelpers.h"
#include "cxPatientModelService.h"
#include "cxVolumeHelpers.h"
#include "cxVisServices.h"
#include "cxMeshesFromLabelsFilter.h"

namespace cx
{
BinaryThinningImageFilter3DFilter::BinaryThinningImageFilter3DFilter(VisServicesPtr services) :
	FilterImpl(services)
{
}

QString BinaryThinningImageFilter3DFilter::getName() const
{
	return "Centerline";
}

QString BinaryThinningImageFilter3DFilter::getType() const
{
	return "binary_thinning_image_filter_3d_filter";
}

QString BinaryThinningImageFilter3DFilter::getHelp() const
{
	return "<html>"
	        "<h3>itk::BinaryThinningImageFilter3D</h3>"
	        "<p>"
	        "This filter computes one-pixel-wide skeleton of a 3D input image."
	        "</p><p>"
	        "This class is parametrized over the type of the input image "
	        "and the type of the output image."
	        "</p><p>"
	        "The input is assumed to be a binary image. All non-zero valued voxels "
	        "are set to 1 internally to simplify the computation. The filter will "
	        "produce a skeleton of the object.  The output background values are 0, "
	        "and the foreground values are 1."
	        "</p><p>"
	        "A 26-neighbourhood configuration is used for the foreground and a "
	        "6-neighbourhood configuration for the background. Thinning is performed "
	        "symmetrically in order to guarantee that the skeleton lies medial within "
	        "the object."
	        "</p><p>"
	        "This filter is a parallel thinning algorithm and is an implementation "
	        "of the algorithm described in:"
	        "</p><p>"
	        "T.C. Lee, R.L. Kashyap, and C.N. Chu.<br>"
	        "Building skeleton models via 3-D medial surface/axis thinning algorithms.<br>"
	        "Computer Vision, Graphics, and Image Processing, 56(6):462--478, 1994."
	        "</p></html>";
}

ColorPropertyBasePtr BinaryThinningImageFilter3DFilter::getColorOption(QDomElement root)
{
	return ColorProperty::initialize("Color", "",
	                                            "Color of output model.",
	                                            QColor("green"), root);
}

BoolPropertyPtr BinaryThinningImageFilter3DFilter::getLabeledVolumeOption(QDomElement root)
{
	return BoolProperty::initialize("Labeled volume", "",
																	"Select to generate seperate centerlines for each label in input volume.",
																	 false, root);
}

void BinaryThinningImageFilter3DFilter::createOptions()
{
	mOptionsAdapters.push_back(this->getColorOption(mOptions));
	mOptionsAdapters.push_back(this->getLabeledVolumeOption(mOptions));
}

void BinaryThinningImageFilter3DFilter::createInputTypes()
{
	SelectDataStringPropertyBasePtr temp;

	temp = StringPropertySelectImage::New(mServices->patient());
	temp->setValueName("Input");
	temp->setHelp("Select binary volume input for thinning");
	//    connect(temp.get(), SIGNAL(dataChanged(QString)), this, SLOT(imageChangedSlot(QString)));
	mInputTypes.push_back(temp);
}

void BinaryThinningImageFilter3DFilter::createOutputTypes()
{
	SelectDataStringPropertyBasePtr temp;

	temp = StringPropertySelectMesh::New((mServices->patient()));
	temp->setValueName("Output");
	temp->setHelp("Output centerline model");
	mOutputTypes.push_back(temp);
}

bool BinaryThinningImageFilter3DFilter::preProcess()
{
	bool retval = FilterImpl::preProcess();
	if (!retval)
		return false;

	ImagePtr input = this->getCopiedInputImage();
	if (!input)
		return false;

	if (input->getMax() != 1 || input->getMin() != 0)
	{
		reportWarning(QString("Centerline: Input image %1 must be binary, aborting.").arg(input->getName()));
		return false;
	}

	return true;
}

bool BinaryThinningImageFilter3DFilter::execute()
{
	ImagePtr input = this->getCopiedInputImage();
	if (!input)
		return false;

	//      report(QString("Creating centerline from \"%1\"...").arg(input->getName()));

	BoolPropertyPtr labeledVolumeOption = this->getLabeledVolumeOption(mCopiedOptions);
	mRawResult =  this->execute(input, labeledVolumeOption->getValue());
	if (mRawResult.empty())
		return false;
	else
		return true;
}

std::vector<vtkImageDataPtr> BinaryThinningImageFilter3DFilter::execute(ImagePtr input, bool labeledVolume)
{

	std::vector<vtkImageDataPtr> retval;

	itkImageType::ConstPointer itkImage = AlgorithmHelper::getITKfromSSCImage(input);

	int minValue = input->getMin();
	int maxValue = input->getMax();
	int numberOfLabels = maxValue - (minValue - 1);

	if(!labeledVolume)
	{
		if (maxValue != 1 || minValue != 0)
			return retval;
	}
	else if((numberOfLabels >= 500))
	{
		CX_LOG_WARNING() << "Too many labes found in centerline filter (" << numberOfLabels << " labels). Aborting.";
		return retval;
	}
	else if(numberOfLabels >= 50)
		CX_LOG_WARNING() << "Many labes found in centerline filter (" << numberOfLabels << " labels).";

	typedef itk::BinaryThresholdImageFilter<itkImageType, itkImageType> thresholdFilterType;
	thresholdFilterType::Pointer thresholdFilter = thresholdFilterType::New();
	thresholdFilter->SetInput(itkImage);
	thresholdFilter->SetOutsideValue(0);
	thresholdFilter->SetInsideValue(1);
	for(int value=minValue+1; value<=maxValue; value++)
	{
		thresholdFilter->SetLowerThreshold(value);
		thresholdFilter->SetUpperThreshold(value);
		thresholdFilter->Update();
		itkImageType::ConstPointer itkBinaryImage = thresholdFilter->GetOutput();

		//Centerline extraction
		typedef itk::BinaryThinningImageFilter3D<itkImageType, itkImageType> centerlineFilterType;
		centerlineFilterType::Pointer centerlineFilter = centerlineFilterType::New();
		centerlineFilter->SetInput(itkBinaryImage);
		centerlineFilter->Update();
		itkImageType::ConstPointer itkOutputImage = centerlineFilter->GetOutput();

		//Convert ITK to VTK
		itkToVtkFilterType::Pointer itkToVtkFilter = itkToVtkFilterType::New();
		itkToVtkFilter->SetInput(itkOutputImage);
		itkToVtkFilter->Update();

		vtkImageDataPtr rawResult = vtkImageDataPtr::New();
		rawResult->DeepCopy(itkToVtkFilter->GetOutput());
		retval.push_back(rawResult);
	}

	return retval;
}

bool BinaryThinningImageFilter3DFilter::postProcess()
{
	bool success = false;
	if(mRawResult.empty())
		return success;

	ColorPropertyBasePtr outputColor = this->getColorOption(mCopiedOptions);

	ImagePtr input = this->getCopiedInputImage();

	int numberOfCenterlines = mRawResult.size();
	for(int i=0; i<numberOfCenterlines; i++)
	{
		ImagePtr outImage = createDerivedImage(mServices->patient(),
											 input->getUid() + "_cl_temp%1", input->getName()+" cl_temp%1",
											 mRawResult[i], input);

		mRawResult[i] = NULL;
		outImage->resetTransferFunctions();

		//automatically generate a mesh from the centerline
		vtkPolyDataPtr centerlinePolyData = SeansVesselReg::extractPolyData(outImage, 1, 0);

		QString uid = input->getUid() + "_cl%1";
		QString name = input->getName()+" cl%1";
		MeshPtr mesh = mServices->patient()->createSpecificData<Mesh>(uid, name);
		mesh->setVtkPolyData(centerlinePolyData);
		QColor color = MeshesFromLabelsFilter::generateColor(outputColor->getValue(), i, numberOfCenterlines);
		mesh->setColor(color);
		mesh->get_rMd_History()->setParentSpace(input->getUid());
		mServices->patient()->insertData(mesh);

		// set first centerline as output in filter
		if(i==0)
			mOutputTypes.front()->setValue(mesh->getUid());

		success = true;
	}

	return success;
}



} // namespace cx

