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

#include "cxContourFilter.h"

#include <vtkImageShrink3D.h>
#include <vtkMarchingCubes.h>
#include <vtkWindowedSincPolyDataFilter.h>
#include <vtkTriangleFilter.h>
#include <vtkDecimatePro.h>
#include <vtkPolyDataNormals.h>
#include <vtkImageData.h>

#include "sscMessageManager.h"
#include "sscDataManager.h"
#include "sscUtilHelpers.h"
#include "sscMesh.h"
#include "sscRegistrationTransform.h"
#include "sscTypeConversions.h"

#include "sscImage.h"
#include "sscMesh.h"
#include "sscDataManager.h"
#include "sscTypeConversions.h"
#include "cxPatientData.h"
#include "cxDataInterface.h"
#include "cxPatientService.h"
#include "cxRepManager.h"


namespace cx
{


QString ContourFilter::getName() const
{
	return "Contour";
}

QString ContourFilter::getType() const
{
	return "ContourFilter";
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

ssc::BoolDataAdapterXmlPtr ContourFilter::getReduceResolutionOption(QDomElement root)
{
	return ssc::BoolDataAdapterXml::initialize("Reduce input", "",
	                                           "Reduce input volumes resolution by a factor of 2 in all directions.", false, root);
}

ssc::BoolDataAdapterXmlPtr ContourFilter::getSmoothingOption(QDomElement root)
{
	return ssc::BoolDataAdapterXml::initialize("Smoothing", "",
	                                           "Smooth the output contour", true, root);
}

ssc::BoolDataAdapterXmlPtr ContourFilter::getPreserveTopologyOption(QDomElement root)
{
	return ssc::BoolDataAdapterXml::initialize("Preserve mesh topology", "",
	                                           "Preserve mesh topology during reduction", true, root);
}

ssc::DoubleDataAdapterXmlPtr ContourFilter::getSurfaceThresholdOption(QDomElement root)
{
	ssc::DoubleDataAdapterXmlPtr retval = ssc::DoubleDataAdapterXml::initialize("Threshold", "",
	                                                                            "Values from this threshold and above will be included",
	                                                                            100.0, ssc::DoubleRange(-1000, 1000, 1), 0, root);
	retval->setAddSlider(true);
	return retval;
}

ssc::DoubleDataAdapterXmlPtr ContourFilter::getDecimationOption(QDomElement root)
{
	ssc::DoubleDataAdapterXmlPtr retval = ssc::DoubleDataAdapterXml::initialize("Decimation %", "",
	                                                                            "Reduce number of triangles in output surface",
	                                                                            0.8, ssc::DoubleRange(0, 1, 0.01), 0, root);
	retval->setInternal2Display(100);
	return retval;
}

ssc::ColorDataAdapterXmlPtr ContourFilter::getColorOption(QDomElement root)
{
	return ssc::ColorDataAdapterXml::initialize("Color", "",
	                                            "Color of output model.",
	                                            QColor("green"), root);
}

void ContourFilter::createOptions(QDomElement root)
{
	mReduceResolutionOption = this->getReduceResolutionOption(root);
	mOptionsAdapters.push_back(mReduceResolutionOption);

	mSurfaceThresholdOption = this->getSurfaceThresholdOption(root);
	connect(mSurfaceThresholdOption.get(), SIGNAL(changed()), this, SLOT(thresholdSlot()));
	mOptionsAdapters.push_back(mSurfaceThresholdOption);

	mOptionsAdapters.push_back(this->getSmoothingOption(root));
	mOptionsAdapters.push_back(this->getDecimationOption(root));
	mOptionsAdapters.push_back(this->getPreserveTopologyOption(root));

	mOptionsAdapters.push_back(this->getColorOption(root));
}

void ContourFilter::createInputTypes()
{
	SelectDataStringDataAdapterBasePtr temp;

	temp = SelectImageStringDataAdapter::New();
	temp->setValueName("Input");
	temp->setHelp("Select image input for contouring");
	connect(temp.get(), SIGNAL(dataChanged(QString)), this, SLOT(imageChangedSlot(QString)));
	mInputTypes.push_back(temp);
}

void ContourFilter::createOutputTypes()
{
	SelectDataStringDataAdapterBasePtr temp;

	temp = SelectMeshStringDataAdapter::New();
	temp->setValueName("Output");
	temp->setHelp("Output contour");
	mOutputTypes.push_back(temp);
}

void ContourFilter::setActive(bool on)
{
	FilterImpl::setActive(on);

	if (!mActive)
		RepManager::getInstance()->getThresholdPreview()->removePreview();
}

void ContourFilter::imageChangedSlot(QString uid)
{
	ssc::ImagePtr image = ssc::dataManager()->getImage(uid);
	if(!image)
		return;

	this->updateThresholdFromImageChange(uid, mSurfaceThresholdOption);

//	mSurfaceThresholdOption->setValueRange(ssc::DoubleRange(image->getMin(), image->getMax(), 1));
//	mSurfaceThresholdOption->setValue(image->getRange() / 2 + image->getMin());
//	int oldValue = mSurfaceThresholdOption->getValue();
//	// avoid reset if old value is still within range
//	if ((image->getMin() > oldValue )||( oldValue > image->getMax()))
//	{
//		int initValue = + image->getMin() + image->getRange()/2 ;
//		mSurfaceThresholdOption->setValue(initValue);
//	}
	RepManager::getInstance()->getThresholdPreview()->removePreview();

	int extent[6];
	image->getBaseVtkImageData()->GetExtent(extent);
	mReduceResolutionOption->setHelp("Current input resolution: " + qstring_cast(extent[1])
	                                 + " " + qstring_cast(extent[3]) + " " + qstring_cast(extent[5])
	                                 + " (If checked: " + qstring_cast(extent[1]/2)+ " " + qstring_cast(extent[3]/2) + " "
	                                 + qstring_cast(extent[5]/2) + ")");
}

void ContourFilter::thresholdSlot()
{
	//    std::cout << "ContourFilter::thresholdSlot() " << mActive << std::endl;
	if (mActive)
	{
		ssc::ImagePtr image = boost::shared_dynamic_cast<ssc::Image>(mInputTypes[0]->getData());
		RepManager::getInstance()->getThresholdPreview()->setPreview(image, mSurfaceThresholdOption->getValue());
	}
}

bool ContourFilter::preProcess()
{
	RepManager::getInstance()->getThresholdPreview()->removePreview();
	return FilterImpl::preProcess();
}

bool ContourFilter::execute()
{
	ssc::ImagePtr input = this->getCopiedInputImage();
	if (!input)
		return false;

	//    std::cout << "ContourFilter::execute : " << mCopiedOptions.ownerDocument().toString() << std::endl;

	ssc::BoolDataAdapterXmlPtr reduceResolutionOption = this->getReduceResolutionOption(mCopiedOptions);
	ssc::BoolDataAdapterXmlPtr smoothingOption = this->getSmoothingOption(mCopiedOptions);
	ssc::BoolDataAdapterXmlPtr preserveTopologyOption = this->getPreserveTopologyOption(mCopiedOptions);
	ssc::DoubleDataAdapterXmlPtr surfaceThresholdOption = this->getSurfaceThresholdOption(mCopiedOptions);
	ssc::DoubleDataAdapterXmlPtr decimationOption = this->getDecimationOption(mCopiedOptions);

	//    ssc::messageManager()->sendInfo(QString("Creating contour from \"%1\"...").arg(input->getName()));

	//Shrink input volume
	vtkImageShrink3DPtr shrinker = vtkImageShrink3DPtr::New();
	if(reduceResolutionOption->getValue())
	{
		std::cout << "smooth" << std::endl;
		shrinker->SetInput(input->getBaseVtkImageData());
		shrinker->SetShrinkFactors(2,2,2);
		shrinker->Update();
	}

	// Find countour
	vtkMarchingCubesPtr convert = vtkMarchingCubesPtr::New();
	if(reduceResolutionOption->getValue())
		convert->SetInput(shrinker->GetOutput());
	else
		convert->SetInput(input->getBaseVtkImageData());

	convert->SetValue(0, surfaceThresholdOption->getValue());
	convert->Update();

	vtkPolyDataPtr cubesPolyData = vtkPolyDataPtr::New();
	cubesPolyData = convert->GetOutput();
	std::cout << "convert->GetOutput(); " << cubesPolyData.GetPointer() << std::endl;

	// Smooth surface model
	vtkWindowedSincPolyDataFilterPtr smoother = vtkWindowedSincPolyDataFilterPtr::New();
	if(smoothingOption->getValue())
	{
		smoother->SetInput(cubesPolyData);
		smoother->Update();
		cubesPolyData = smoother->GetOutput();
	}

	//Create a surface of triangles

	//Decimate surface model (remove a percentage of the polygons)
	vtkTriangleFilterPtr trifilt = vtkTriangleFilterPtr::New();
	vtkDecimateProPtr deci = vtkDecimateProPtr::New();
	vtkPolyDataNormalsPtr normals = vtkPolyDataNormalsPtr::New();
	if (decimationOption->getValue() > 0.000001)
	{
		trifilt->SetInput(cubesPolyData);
		trifilt->Update();
		deci->SetInput(trifilt->GetOutput());
		deci->SetTargetReduction(decimationOption->getValue());
		deci->SetPreserveTopology(preserveTopologyOption->getValue());
		//    deci->PreserveTopologyOn();
		deci->Update();
		cubesPolyData = deci->GetOutput();
	}

	normals->SetInput(cubesPolyData);
	normals->Update();

	cubesPolyData->DeepCopy(normals->GetOutput());

	mRawResult =  cubesPolyData;
	return true;
}

void ContourFilter::postProcess()
{
	if (!mRawResult)
		return;

	ssc::ImagePtr input = this->getCopiedInputImage();

	if (!input)
		return;

	QString uid = input->getUid() + "_ge%1";
	QString name = input->getName()+" ge%1";
	ssc::MeshPtr output = ssc::dataManager()->createMesh(mRawResult, uid, name, "Images");
	if (!output)
		return;

	output->get_rMd_History()->setRegistration(input->get_rMd());
	output->get_rMd_History()->setParentSpace(input->getUid());

	ssc::ColorDataAdapterXmlPtr colorOption = this->getColorOption(mOptions);
	output->setColor(colorOption->getValue());

	ssc::dataManager()->loadData(output);
	ssc::dataManager()->saveData(output, patientService()->getPatientData()->getActivePatientFolder());

	// set output
	mOutputTypes.front()->setValue(output->getUid());
}

} // namespace cx
