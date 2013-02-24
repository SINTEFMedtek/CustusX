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

#include "cxBinaryThinningImageFilter3DFilter.h"

#include <itkBinaryThinningImageFilter3D.h>

#include "sscMessageManager.h"
#include "sscDataManager.h"
#include "sscRegistrationTransform.h"
#include "sscMesh.h"
#include "sscImage.h"
#include "sscColorDataAdapterXml.h"
#include "vesselReg/SeansVesselReg.hxx"
#include "cxPatientService.h"
#include "cxPatientData.h"
#include "cxSelectDataStringDataAdapter.h"
#include "cxAlgorithmHelpers.h"

namespace cx
{

QString BinaryThinningImageFilter3DFilter::getName() const
{
	return "Centerline";
}

QString BinaryThinningImageFilter3DFilter::getType() const
{
	return "BinaryThinningImageFilter3DFilter";
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

ssc::ColorDataAdapterPtr BinaryThinningImageFilter3DFilter::getColorOption(QDomElement root)
{
	return ssc::ColorDataAdapterXml::initialize("Color", "",
	                                            "Color of output model.",
	                                            QColor("green"), root);
}

void BinaryThinningImageFilter3DFilter::createOptions()
{
	mOptionsAdapters.push_back(this->getColorOption(mOptions));
}

void BinaryThinningImageFilter3DFilter::createInputTypes()
{
	SelectDataStringDataAdapterBasePtr temp;

	temp = SelectImageStringDataAdapter::New();
	temp->setValueName("Input");
	temp->setHelp("Select binary volume input for thinning");
	//    connect(temp.get(), SIGNAL(dataChanged(QString)), this, SLOT(imageChangedSlot(QString)));
	mInputTypes.push_back(temp);
}

void BinaryThinningImageFilter3DFilter::createOutputTypes()
{
	SelectDataStringDataAdapterBasePtr temp;

	temp = SelectMeshStringDataAdapter::New();
	temp->setValueName("Output");
	temp->setHelp("Output centerline model");
	mOutputTypes.push_back(temp);
}

bool BinaryThinningImageFilter3DFilter::preProcess()
{
	bool retval = FilterImpl::preProcess();
	if (!retval)
		return false;

	ssc::ImagePtr input = this->getCopiedInputImage();
	if (!input)
		return false;

	if (input->getMax() != 1 || input->getMin() != 0)
	{
		ssc::messageManager()->sendWarning(QString("Centerline: Input image %1 must be binary, aborting.").arg(input->getName()));
		return false;
	}

	return true;
}

bool BinaryThinningImageFilter3DFilter::execute()
{
	ssc::ImagePtr input = this->getCopiedInputImage();
	if (!input)
		return false;

	if (input->getMax() != 1 || input->getMin() != 0)
	{
		return false;
	}

	//      ssc::messageManager()->sendInfo(QString("Creating centerline from \"%1\"...").arg(input->getName()));

	itkImageType::ConstPointer itkImage = AlgorithmHelper::getITKfromSSCImage(input);

	//Centerline extraction
	typedef itk::BinaryThinningImageFilter3D<itkImageType, itkImageType> centerlineFilterType;
	centerlineFilterType::Pointer centerlineFilter = centerlineFilterType::New();
	centerlineFilter->SetInput(itkImage);
	centerlineFilter->Update();
	itkImage = centerlineFilter->GetOutput();

	//Convert ITK to VTK
	itkToVtkFilterType::Pointer itkToVtkFilter = itkToVtkFilterType::New();
	itkToVtkFilter->SetInput(itkImage);
	itkToVtkFilter->Update();

	vtkImageDataPtr rawResult = vtkImageDataPtr::New();
	rawResult->DeepCopy(itkToVtkFilter->GetOutput());

	mRawResult =  rawResult;
	return true;
}

bool BinaryThinningImageFilter3DFilter::postProcess()
{
	bool success = false;
	if(!mRawResult)
		return success;

	ssc::ColorDataAdapterPtr outputColor = this->getColorOption(mCopiedOptions);

	ssc::ImagePtr input = this->getCopiedInputImage();

	ssc::ImagePtr outImage = ssc::dataManager()->createDerivedImage(mRawResult,input->getUid() + "_cl_temp%1", input->getName()+" cl_temp%1", input);
	mRawResult = NULL;
	outImage->resetTransferFunctions();

	//automatically generate a mesh from the centerline
	vtkPolyDataPtr centerlinePolyData = ssc::SeansVesselReg::extractPolyData(outImage, 1, 0);

	QString uid = input->getUid() + "_cl%1";
	QString name = input->getName()+" cl%1";
	ssc::MeshPtr mesh = ssc::dataManager()->createMesh(centerlinePolyData, uid, name, "Images");
	mesh->setColor(outputColor->getValue());
	mesh->get_rMd_History()->setParentSpace(input->getUid());
	ssc::dataManager()->loadData(mesh);
	ssc::dataManager()->saveMesh(mesh, patientService()->getPatientData()->getActivePatientFolder());

	// set output
	mOutputTypes.front()->setValue(mesh->getUid());
	success = true;

	return success;
}



} // namespace cx

