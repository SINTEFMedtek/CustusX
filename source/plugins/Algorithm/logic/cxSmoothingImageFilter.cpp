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

#include "cxSmoothingImageFilter.h"

#include "cxAlgorithmHelpers.h"

#include "sscDataManager.h"
#include "sscUtilHelpers.h"
#include "sscRegistrationTransform.h"
#include "sscStringDataAdapterXml.h"
#include "sscDoubleDataAdapterXml.h"
#include "sscBoolDataAdapterXml.h"
#include "sscTypeConversions.h"
#include "sscImage.h"

#include "cxPatientService.h"
#include "cxPatientData.h"

namespace cx
{

QString SmoothingImageFilter::getName() const
{
    return "Smoothing";
}

QString SmoothingImageFilter::getType() const
{
    return "SmoothingImageFilter";
}

QString SmoothingImageFilter::getHelp() const
{
    return "<html>"
            "<h3>Smoothing.</h3>"
            "<p>Wrapper for a itk::SmoothingRecursiveGaussianImageFilter.</p>"
            "<p>Computes the smoothing of an image by convolution with "
            "the Gaussian kernels implemented as IIR filters."
            "This filter is implemented using the recursive gaussian filters.</p>"
            "</html>";
}

ssc::DoubleDataAdapterXmlPtr SmoothingImageFilter::getSigma(QDomElement root)
{
    return ssc::DoubleDataAdapterXml::initialize("Smoothing sigma", "",
        "Used for smoothing the segmented volume. Measured in units of image spacing.",
        0.10, ssc::DoubleRange(0, 5, 0.01), 2);
}

void SmoothingImageFilter::createOptions(QDomElement root)
{
    mOptionsAdapters.push_back(this->getSigma(root));
}

void SmoothingImageFilter::createInputTypes()
{
    SelectDataStringDataAdapterBasePtr temp;

    temp = SelectImageStringDataAdapter::New();
    temp->setValueName("Input");
    temp->setHelp("Select image input for smoothing");
    mInputTypes.push_back(temp);
}

void SmoothingImageFilter::createOutputTypes()
{
    SelectDataStringDataAdapterBasePtr temp;

    temp = SelectDataStringDataAdapter::New();
    temp->setValueName("Output");
    temp->setHelp("Output smoothed image");
    mOutputTypes.push_back(temp);
}

bool SmoothingImageFilter::execute()
{
    ssc::ImagePtr input = this->getCopiedInputImage();
    if (!input)
        return false;

    ssc::DoubleDataAdapterXmlPtr sigma = this->getSigma(mCopiedOptions);

    itkImageType::ConstPointer itkImage = AlgorithmHelper::getITKfromSSCImage(input);

  typedef itk::SmoothingRecursiveGaussianImageFilter<itkImageType, itkImageType> smoothingFilterType;
  smoothingFilterType::Pointer smoohingFilter = smoothingFilterType::New();
  smoohingFilter->SetSigma(sigma->getValue());
  smoohingFilter->SetInput(itkImage);
  smoohingFilter->Update();
  itkImage = smoohingFilter->GetOutput();

    //Convert ITK to VTK
    itkToVtkFilterType::Pointer itkToVtkFilter = itkToVtkFilterType::New();
    itkToVtkFilter->SetInput(itkImage);
    itkToVtkFilter->Update();

    vtkImageDataPtr rawResult = vtkImageDataPtr::New();
    rawResult->DeepCopy(itkToVtkFilter->GetOutput());
    // TODO: possible memory problem here - check debug mem system of itk/vtk

    mRawResult =  rawResult;
    return true;
}

void SmoothingImageFilter::postProcess()
{
    if (!mRawResult)
        return;

    ssc::ImagePtr input = this->getCopiedInputImage();

    if (!input)
        return;

    QString uid = input->getUid() + "_sm%1";
    QString name = input->getName()+" sm%1";
    ssc::ImagePtr output = ssc::dataManager()->createDerivedImage(mRawResult,uid, name, input);
    if (!output)
        return;

//    output->resetTransferFunctions();
    ssc::dataManager()->loadData(output);
    ssc::dataManager()->saveImage(output, patientService()->getPatientData()->getActivePatientFolder());

    // set output
    mOutputTypes.front()->setValue(output->getUid());
}


} // namespace cx

