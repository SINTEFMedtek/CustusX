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

#include "cxVNNclReconstructionMethodService.h"
#include "cxLogger.h"
#include "recConfig.h"
#include "cxDataLocations.h"

namespace cx
{

VNNclReconstructionMethodService::VNNclReconstructionMethodService(ctkPluginContext* context) :
		ReconstructionMethodService()
{
    mAlgorithm = VNNclAlgorithmPtr(new VNNclAlgorithm);

    mMethods.push_back("VNN");
    mMethods.push_back("VNN2");
    mMethods.push_back("DW");
    mMethods.push_back("Anisotropic");
    mPlaneMethods.push_back("Heuristic");
    mPlaneMethods.push_back("Closest");
    mPlaneMethods.push_back("Closest ver2");
}

VNNclReconstructionMethodService::~VNNclReconstructionMethodService()
{
}

void VNNclReconstructionMethodService::enableProfiling()
{
    mAlgorithm->setProfiling(true);
}

double VNNclReconstructionMethodService::getKernelExecutionTime()
{
    return mAlgorithm->getKernelExecutionTime();
}

QString VNNclReconstructionMethodService::getName() const
{
    return "VNNcl";
}

std::vector<PropertyPtr> VNNclReconstructionMethodService::getSettings(QDomElement root)
{
    std::vector<PropertyPtr> retval;

    retval.push_back(this->getMethodOption(root));
    retval.push_back(this->getRadiusOption(root));
    retval.push_back(this->getPlaneMethodOption(root));
    retval.push_back(this->getMaxPlanesOption(root));
    retval.push_back(this->getNStartsOption(root));
    retval.push_back(this->getNewnessWeightOption(root));
    retval.push_back(this->getBrightnessWeightOption(root));
    return retval;
}

bool VNNclReconstructionMethodService::reconstruct(ProcessedUSInputDataPtr input, vtkImageDataPtr outputData, QDomElement settings)
{
    int nClosePlanes = getMaxPlanesOption(settings)->getValue();

    int method = getMethodID(settings);
    float radius = getRadiusOption(settings)->getValue();
    int planeMethod = getPlaneMethodID(settings);
    int nStarts = getNStartsOption(settings)->getValue();
    float newnessWeight = getNewnessWeightOption(settings)->getValue();
    float brightnessWeight = getBrightnessWeightOption(settings)->getValue();

    report(
            QString("Method: %1, radius: %2, planeMethod: %3, nClosePlanes: %4, nPlanes: %5, nStarts: %6 ").arg(method).arg(
                    radius).arg(planeMethod).arg(nClosePlanes).arg(input->getDimensions()[2]).arg(nStarts));

	QString kernel = DataLocations::findConfigFilePath("/kernels.cl", "/shaders", VNNCL_KERNEL_PATH);
	if (!mAlgorithm->initCL(kernel, nClosePlanes, input->getDimensions()[2], method, planeMethod, nStarts, newnessWeight, brightnessWeight))
        return false;

    bool ret = mAlgorithm->reconstruct(input, outputData, radius, nClosePlanes);

    return ret;
}

StringPropertyPtr VNNclReconstructionMethodService::getMethodOption(QDomElement root)
{
    QStringList methods;
    for (std::vector<QString>::iterator it = mMethods.begin(); it != mMethods.end(); ++it)
    {
        QString method = *it;
        methods << method;
    }
    return StringProperty::initialize("Method", "", "Which algorithm to use for reconstruction", methods[2],
            methods, root);
}

DoublePropertyPtr VNNclReconstructionMethodService::getNewnessWeightOption(QDomElement root)
{
    return DoubleProperty::initialize("Newness weight", "", "Newness weight", 0, DoubleRange(0.0, 10, 0.1), 1,
            root);
}

DoublePropertyPtr VNNclReconstructionMethodService::getBrightnessWeightOption(QDomElement root)
{
    return DoubleProperty::initialize("Brightness weight", "", "Brightness weight", 1, DoubleRange(0.0, 10, 0.1),
            1, root);
}

StringPropertyPtr VNNclReconstructionMethodService::getPlaneMethodOption(QDomElement root)
{
    QStringList methods;
    for (std::vector<QString>::iterator it = mPlaneMethods.begin(); it != mPlaneMethods.end(); ++it)
    {
        QString method = *it;
        methods << method;
    }
    return StringProperty::initialize("Plane method", "", "Which method to use for finding close planes",
            methods[0], methods, root);
}

DoublePropertyPtr VNNclReconstructionMethodService::getRadiusOption(QDomElement root)
{
    return DoubleProperty::initialize("Radius (mm)", "", "Radius of kernel. mm.", 3, DoubleRange(0.1, 10, 0.1), 1,
            root);
}

DoublePropertyPtr VNNclReconstructionMethodService::getMaxPlanesOption(QDomElement root)
{
    return DoubleProperty::initialize("nPlanes", "", "Number of planes to include in closest planes", 10,
            DoubleRange(1, 200, 1), 0, root);
}

DoublePropertyPtr VNNclReconstructionMethodService::getNStartsOption(QDomElement root)
{
    return DoubleProperty::initialize("nStarts", "", "Number of starts for multistart searchs", 16,
            DoubleRange(1, 16, 1), 0, root);
}

int VNNclReconstructionMethodService::getMethodID(QDomElement root)
{
    return find(mMethods.begin(), mMethods.end(), this->getMethodOption(root)->getValue()) - mMethods.begin();
}

int VNNclReconstructionMethodService::getPlaneMethodID(QDomElement root)
{
    return find(mPlaneMethods.begin(), mPlaneMethods.end(), this->getPlaneMethodOption(root)->getValue())
            - mPlaneMethods.begin();
}


} /* namespace cx */	
