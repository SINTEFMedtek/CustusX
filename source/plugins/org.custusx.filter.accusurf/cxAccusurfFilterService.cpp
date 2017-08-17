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

#include "cxAccusurfFilterService.h"

#include <ctkPluginContext.h>
#include "cxImage.h"

#include "cxAlgorithmHelpers.h"
#include "cxSelectDataStringProperty.h"

#include "cxUtilHelpers.h"
#include "cxRegistrationTransform.h"
#include "cxStringProperty.h"
#include "cxDoubleProperty.h"
#include "cxBoolProperty.h"
#include "cxTypeConversions.h"
#include "cxVolumeHelpers.h"
#include "cxAccusurf.h"
#include "cxPatientModelService.h"
#include "cxPointMetric.h"
#include "cxVisServices.h"
#include "cxStringPropertySelectPointMetric.h"
#include "cxPatientModelServiceProxy.h"
#include "cxViewService.h"
#include "cxLog.h"


namespace cx
{

AccusurfFilter::AccusurfFilter(VisServicesPtr services) :
	FilterImpl(services)
{
}

QString AccusurfFilter::getName() const
{
	return "ACCuSurf";
}

QString AccusurfFilter::getType() const
{
	return "accusurf_filter";
}

QString AccusurfFilter::getHelp() const
{
	return "<html>"
			"<h3>Accusurf</h3>"
    "<p>Filter to generate an ACCuSurf volume to be usen in navigated bronchoscopy."
			"The ACCuSurf is generated based on a standard thorax CT and a route-to-target centerline."
			"</p>"
			"</html>";
}


void AccusurfFilter::createOptions()
{
    mOptionsAdapters.push_back(this->getAccusurfThicknessUp(mOptions));
    mOptionsAdapters.push_back(this->getAccusurfThicknessDown(mOptions));
}

void AccusurfFilter::createInputTypes()
{
	StringPropertySelectMeshPtr centerline;
	centerline = StringPropertySelectMesh::New(mServices->patient());
    centerline->setValueName("Route-to-Target");
	centerline->setHelp("Select centerline");
	mInputTypes.push_back(centerline);

    SelectDataStringPropertyBasePtr image;
    image = StringPropertySelectImage::New(mServices->patient());
    image->setValueName("Input image");
    image->setHelp("Select image input for ACCuSurf.");
    //connect(image.get(), SIGNAL(dataChanged(QString)), this, SLOT(imageChangedSlot(QString)));
    mInputTypes.push_back(image);

}

void AccusurfFilter::createOutputTypes()
{
    SelectDataStringPropertyBasePtr image;

    image = StringPropertySelectData::New(mServices->patient());
    image->setValueName("Output");
    image->setHelp("Output ACCuSurf");
    mOutputTypes.push_back(image);
}


bool AccusurfFilter::execute()
{
    mAccusurf.reset(new Accusurf());

	MeshPtr mesh = boost::dynamic_pointer_cast<StringPropertySelectMesh>(mInputTypes[0])->getMesh();

    ImagePtr inputImage = this->getCopiedInputImage(1);
    if (!inputImage)
        return false;

	vtkPolyDataPtr route_d_image = mesh->getTransformedPolyDataCopy((inputImage->get_rMd().inverse())*mesh->get_rMd());
    mAccusurf->setRoutePositions(route_d_image);
    mAccusurf->setInputImage(inputImage);

    DoublePropertyPtr thicknessUp =this->getAccusurfThicknessUp(mOptions);
    DoublePropertyPtr thicknessDown =this->getAccusurfThicknessDown(mOptions);
    mAccusurf->setThickness(thicknessUp->getValue(), thicknessDown->getValue());

    mAccusurfImage = mAccusurf->createAccusurfImage();

	return true;
}

bool AccusurfFilter::postProcess()
{
    if (!mAccusurfImage)
        return false;

    ImagePtr input = this->getCopiedInputImage(1);

    if (!input)
        return false;

    QString uid = input->getUid() + "_ACCuSurf%1";
    QString name = input->getName()+" ACCuSurf%1";
    ImagePtr output = createDerivedImage(mServices->patient(),
                                         uid, name,
                                         mAccusurfImage, input);
    output->mergevtkSettingsIntosscTransform();

    mAccusurfImage = NULL;
    if (!output)
        return false;

    mServices->patient()->insertData(output);

    // set output
    mOutputTypes.front()->setValue(output->getUid());

    return true;
}


DoublePropertyPtr AccusurfFilter::getAccusurfThicknessUp(QDomElement root)
{
    DoublePropertyPtr retval = DoubleProperty::initialize("ACCuSurf thickness anterior/up (voxels)", "",
    "Set slice thickness up in voxels", 0, DoubleRange(0, 50, 1), 0,
                    root);
    retval->setGuiRepresentation(DoublePropertyBase::grSLIDER);
    return retval;
}

DoublePropertyPtr AccusurfFilter::getAccusurfThicknessDown(QDomElement root)
{
    DoublePropertyPtr retval = DoubleProperty::initialize("ACCuSurf thickness posterior/down (voxels)", "",
    "Set slice thickness down in voxels", 15, DoubleRange(0, 50, 1), 0,
                    root);
    retval->setGuiRepresentation(DoublePropertyBase::grSLIDER);
    return retval;
}


} // namespace cx

