/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxContourFilter.h"

#include <vtkImageShrink3D.h>
#include <vtkMarchingCubes.h>
#include <vtkWindowedSincPolyDataFilter.h>
#include <vtkTriangleFilter.h>
#include <vtkDecimatePro.h>
#include <vtkPolyDataNormals.h>
#include <vtkImageData.h>


#include "cxRegistrationTransform.h"
#include "cxTypeConversions.h"
#include "cxImage.h"
#include "cxMesh.h"
#include "cxDoubleProperty.h"
#include "cxBoolProperty.h"
#include "cxColorProperty.h"
#include "cxSelectDataStringProperty.h"
#include "cxPatientModelService.h"
#include "cxViewService.h"
#include "cxVisServices.h"

namespace cx
{

ContourFilter::ContourFilter(VisServicesPtr services) :
	FilterImpl(services)
{
}

QString ContourFilter::getName() const
{
	return "Contour";
}

QString ContourFilter::getType() const
{
	return "contour_filter";
}

QString ContourFilter::getHelp() const
{
	return "<html>"
	        "<h3>Surfacing.</h3>"
	        "<p><i>Find the surface of a binary volume using marching cubes.</i></p>"
	        "<p>- Optional factor 2 reduction</p>"
	        "<p>- Marching Cubes contouring</p>"
	        "<p>- Optional Windowed Sinc smoothing</p>"
	        "<p>- Decimation of triangles</p>"
           "</html>";
}

QString ContourFilter::getNameSuffix()
{
    return "_ge";
}

BoolPropertyPtr ContourFilter::getReduceResolutionOption(QDomElement root)
{
	return BoolProperty::initialize("Reduce input", "",
	                                           "Reduce input volumes resolution by a factor of 2 in all directions.", false, root);
}

BoolPropertyPtr ContourFilter::getSmoothingOption(QDomElement root)
{
	return BoolProperty::initialize("Smoothing", "",
	                                           "Smooth the output contour", true, root);
}

BoolPropertyPtr ContourFilter::getPreserveTopologyOption(QDomElement root)
{
	return BoolProperty::initialize("Preserve mesh topology", "",
	                                           "Preserve mesh topology during reduction", true, root);
}

DoublePropertyPtr ContourFilter::getSurfaceThresholdOption(QDomElement root)
{
	DoublePropertyPtr retval = DoubleProperty::initialize("Threshold", "",
	                                                                            "Values from this threshold and above will be included",
	                                                                            100.0, DoubleRange(-1000, 1000, 1), 0, root);
	retval->setGuiRepresentation(DoublePropertyBase::grSLIDER);
	return retval;
}

DoublePropertyPtr ContourFilter::getDecimationOption(QDomElement root)
{
	DoublePropertyPtr retval = DoubleProperty::initialize("Decimation %", "",
	                                                                            "Reduce number of triangles in output surface",
	                                                                            0.2, DoubleRange(0, 1, 0.01), 0, root);
	retval->setInternal2Display(100);
	return retval;
}

ColorPropertyPtr ContourFilter::getColorOption(QDomElement root)
{
	return ColorProperty::initialize("Color", "",
	                                            "Color of output model.",
	                                            QColor("green"), root);
}

void ContourFilter::createOptions()
{
	mReduceResolutionOption = this->getReduceResolutionOption(mOptions);
	mOptionsAdapters.push_back(mReduceResolutionOption);

	mSurfaceThresholdOption = this->getSurfaceThresholdOption(mOptions);
	connect(mSurfaceThresholdOption.get(), SIGNAL(changed()), this, SLOT(thresholdSlot()));
	mOptionsAdapters.push_back(mSurfaceThresholdOption);

	mOptionsAdapters.push_back(this->getSmoothingOption(mOptions));
	mOptionsAdapters.push_back(this->getDecimationOption(mOptions));
	mOptionsAdapters.push_back(this->getPreserveTopologyOption(mOptions));

	mOptionsAdapters.push_back(this->getColorOption(mOptions));
}

void ContourFilter::createInputTypes()
{
	SelectDataStringPropertyBasePtr temp;

	temp = StringPropertySelectImage::New(mServices->patient());
	temp->setValueName("Input");
	temp->setHelp("Select image input for contouring");
	connect(temp.get(), SIGNAL(dataChanged(QString)), this, SLOT(imageChangedSlot(QString)));
	mInputTypes.push_back(temp);
}

void ContourFilter::createOutputTypes()
{
	SelectDataStringPropertyBasePtr temp;

	temp = StringPropertySelectMesh::New(mServices->patient());
	temp->setValueName("Output");
	temp->setHelp("Output contour");
	mOutputTypes.push_back(temp);
}

void ContourFilter::setActive(bool on)
{
	FilterImpl::setActive(on);

	if (!mActive)
		this->stopPreview();
}

void ContourFilter::stopPreview()
{
	if(mPreviewImage)
		mPreviewImage->stopThresholdPreview();
	mPreviewImage.reset();
}

void ContourFilter::imageChangedSlot(QString uid)
{
	ImagePtr image = mServices->patient()->getData<Image>(uid);
	if(!image)
		return;

	this->updateThresholdFromImageChange(uid, mSurfaceThresholdOption);
	this->stopPreview();

	int extent[6];
	image->getBaseVtkImageData()->GetExtent(extent);
	mReduceResolutionOption->setHelp("Current input resolution: " + qstring_cast(extent[1])
	                                 + " " + qstring_cast(extent[3]) + " " + qstring_cast(extent[5])
	                                 + " (If checked: " + qstring_cast(extent[1]/2)+ " " + qstring_cast(extent[3]/2) + " "
	                                 + qstring_cast(extent[5]/2) + ")");
}

void ContourFilter::thresholdSlot()
{
	if (mActive)
	{
		mPreviewImage = boost::dynamic_pointer_cast<Image>(mInputTypes[0]->getData());
		if(mPreviewImage)
		{
			Eigen::Vector2d threshold = Eigen::Vector2d(mSurfaceThresholdOption->getValue(),  mPreviewImage->getMax());
			mPreviewImage->startThresholdPreview(threshold);
		}
	}
}

bool ContourFilter::preProcess()
{
	this->stopPreview();
	return FilterImpl::preProcess();
}

bool ContourFilter::execute()
{
	ImagePtr input = this->getCopiedInputImage();
	if (!input)
		return false;

	//    std::cout << "ContourFilter::execute : " << mCopiedOptions.ownerDocument().toString() << std::endl;

	BoolPropertyPtr reduceResolutionOption = this->getReduceResolutionOption(mCopiedOptions);
	BoolPropertyPtr smoothingOption = this->getSmoothingOption(mCopiedOptions);
	BoolPropertyPtr preserveTopologyOption = this->getPreserveTopologyOption(mCopiedOptions);
	DoublePropertyPtr surfaceThresholdOption = this->getSurfaceThresholdOption(mCopiedOptions);
	DoublePropertyPtr decimationOption = this->getDecimationOption(mCopiedOptions);

	//    report(QString("Creating contour from \"%1\"...").arg(input->getName()));

	mRawResult = this->execute(input->getBaseVtkImageData(),
	                           surfaceThresholdOption->getValue(),
	                           reduceResolutionOption->getValue(),
	                           smoothingOption->getValue(),
	                           preserveTopologyOption->getValue(),
	                           decimationOption->getValue());
	return true;
}

vtkPolyDataPtr ContourFilter::execute(vtkImageDataPtr input,
                                      double threshold,
                                      bool reduceResolution,
                                      bool smoothing,
                                      bool preserveTopology,
                                      double decimation)
{
	if (!input)
		return vtkPolyDataPtr();

	//Shrink input volume
	vtkImageShrink3DPtr shrinker = vtkImageShrink3DPtr::New();
	if(reduceResolution)
	{
//		std::cout << "smooth" << std::endl;
		shrinker->SetInputData(input);
		shrinker->SetShrinkFactors(2,2,2);
		shrinker->Update();
	}

	// Find countour
	vtkMarchingCubesPtr convert = vtkMarchingCubesPtr::New();
	if(reduceResolution)
		convert->SetInputConnection(shrinker->GetOutputPort());
	else
		convert->SetInputData(input);

	convert->SetValue(0, threshold);
	convert->Update();

	vtkPolyDataPtr cubesPolyData = convert->GetOutput();
//	vtkPolyDataPtr cubesPolyData = vtkPolyDataPtr::New();
//	cubesPolyData = convert->GetOutput();
//	std::cout << "convert->GetOutput(); " << cubesPolyData.GetPointer() << std::endl;

	// Smooth surface model
	vtkWindowedSincPolyDataFilterPtr smoother = vtkWindowedSincPolyDataFilterPtr::New();
	if(smoothing)
	{
		smoother->SetInputData(cubesPolyData);
		smoother->SetNumberOfIterations(15);// Higher number = more smoothing
		smoother->SetBoundarySmoothing(false);
		smoother->SetFeatureEdgeSmoothing(false);
		smoother->SetNormalizeCoordinates(true);
		smoother->SetFeatureAngle(120);
		smoother->SetPassBand(0.3);//Lower number = more smoothing
		smoother->Update();
		cubesPolyData = smoother->GetOutput();
	}

	//Create a surface of triangles

	//Decimate surface model (remove a percentage of the polygons)
	vtkTriangleFilterPtr trifilt = vtkTriangleFilterPtr::New();
	vtkDecimateProPtr deci = vtkDecimateProPtr::New();
	vtkPolyDataNormalsPtr normals = vtkPolyDataNormalsPtr::New();
	if (decimation > 0.000001)
	{
		trifilt->SetInputData(cubesPolyData);
		trifilt->Update();
		deci->SetInputConnection(trifilt->GetOutputPort());
		deci->SetTargetReduction(decimation);
		deci->SetPreserveTopology(preserveTopology);
		//    deci->PreserveTopologyOn();
		deci->Update();
		cubesPolyData = deci->GetOutput();
	}

	normals->SetInputData(cubesPolyData);
	normals->Update();

	cubesPolyData->DeepCopy(normals->GetOutput());

	return cubesPolyData;
}

bool ContourFilter::postProcess()
{
	if (!mRawResult)
		return false;

	ImagePtr input = this->getCopiedInputImage();

	if (!input)
		return false;

	ColorPropertyPtr colorOption = this->getColorOption(mOptions);
	MeshPtr output = this->postProcess(mServices->patient(), mRawResult, input, colorOption->getValue());
	mRawResult = NULL;

	if (output)
		mOutputTypes.front()->setValue(output->getUid());

	return true;
}

MeshPtr ContourFilter::postProcess(PatientModelServicePtr patient, vtkPolyDataPtr contour, ImagePtr base, QColor color)
{
	if (!contour || !base)
		return MeshPtr();

    QString uid = base->getUid() + ContourFilter::getNameSuffix() + "%1";
    QString name = base->getName()+ ContourFilter::getNameSuffix() + "%1";
	MeshPtr output = patient->createSpecificData<Mesh>(uid, name);
	output->setVtkPolyData(contour);
	if (!output)
		return MeshPtr();

	output->get_rMd_History()->setRegistration(base->get_rMd());
	output->get_rMd_History()->setParentSpace(base->getUid());

	output->setColor(color);

	patient->insertData(output);

	return output;
}

} // namespace cx
