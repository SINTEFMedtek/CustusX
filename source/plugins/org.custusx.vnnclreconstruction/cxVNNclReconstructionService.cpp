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

#include "cxVNNclReconstructionService.h"
#include "cxReporter.h"
#include "recConfig.h"

namespace cx
{

VNNclReconstructionService::VNNclReconstructionService(ctkPluginContext* context) :
		ReconstructionService()
{
    mAlgorithm = VNNclAlgorithmPtr(new VNNclAlgorithm);

    mMethods.push_back("VNN");
    mMethods.push_back("VNN2");
    mMethods.push_back("DW");
    mMethods.push_back("Anisotropic");
    mPlaneMethods.push_back("Heuristic");
    mPlaneMethods.push_back("Closest");
}

VNNclReconstructionService::~VNNclReconstructionService()
{
}

void VNNclReconstructionService::enableProfiling()
{
    mAlgorithm->setProfiling(true);
}

double VNNclReconstructionService::getKernelExecutionTime()
{
    return mAlgorithm->getKernelExecutionTime();
}

QString VNNclReconstructionService::getName() const
{
    return "VNNcl";
}

std::vector<DataAdapterPtr> VNNclReconstructionService::getSettings(QDomElement root)
{
    std::vector<DataAdapterPtr> retval;

    retval.push_back(this->getMethodOption(root));
    retval.push_back(this->getRadiusOption(root));
    retval.push_back(this->getPlaneMethodOption(root));
    retval.push_back(this->getMaxPlanesOption(root));
    retval.push_back(this->getNStartsOption(root));
    retval.push_back(this->getNewnessWeightOption(root));
    retval.push_back(this->getBrightnessWeightOption(root));
    return retval;
}

bool VNNclReconstructionService::reconstruct(ProcessedUSInputDataPtr input, vtkImageDataPtr outputData, QDomElement settings)
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

    if (!mAlgorithm->initCL(QString(VNNCL_KERNEL_PATH) + "/kernels.cl", nClosePlanes, input->getDimensions()[2], method, planeMethod, nStarts, newnessWeight, brightnessWeight))
        return false;

    bool ret = mAlgorithm->reconstruct(input, outputData, radius, nClosePlanes);

    return ret;
}

StringDataAdapterXmlPtr VNNclReconstructionService::getMethodOption(QDomElement root)
{
    QStringList methods;
    for (std::vector<QString>::iterator it = mMethods.begin(); it != mMethods.end(); ++it)
    {
        QString method = *it;
        methods << method;
    }
    return StringDataAdapterXml::initialize("Method", "", "Which algorithm to use for reconstruction", methods[0],
            methods, root);
}

DoubleDataAdapterXmlPtr VNNclReconstructionService::getNewnessWeightOption(QDomElement root)
{
    return DoubleDataAdapterXml::initialize("Newness weight", "", "Newness weight", 0, DoubleRange(0.0, 10, 0.1), 1,
            root);
}

DoubleDataAdapterXmlPtr VNNclReconstructionService::getBrightnessWeightOption(QDomElement root)
{
    return DoubleDataAdapterXml::initialize("Brightness weight", "", "Brightness weight", 0, DoubleRange(0.0, 10, 0.1),
            1, root);
}

StringDataAdapterXmlPtr VNNclReconstructionService::getPlaneMethodOption(QDomElement root)
{
    QStringList methods;
    for (std::vector<QString>::iterator it = mPlaneMethods.begin(); it != mPlaneMethods.end(); ++it)
    {
        QString method = *it;
        methods << method;
    }
    return StringDataAdapterXml::initialize("Plane method", "", "Which method to use for finding close planes",
            methods[0], methods, root);
}

DoubleDataAdapterXmlPtr VNNclReconstructionService::getRadiusOption(QDomElement root)
{
    return DoubleDataAdapterXml::initialize("Radius (mm)", "", "Radius of kernel. mm.", 1, DoubleRange(0.1, 10, 0.1), 1,
            root);
}

DoubleDataAdapterXmlPtr VNNclReconstructionService::getMaxPlanesOption(QDomElement root)
{
    return DoubleDataAdapterXml::initialize("nPlanes", "", "Number of planes to include in closest planes", 8,
            DoubleRange(1, 200, 1), 0, root);
}

DoubleDataAdapterXmlPtr VNNclReconstructionService::getNStartsOption(QDomElement root)
{
    return DoubleDataAdapterXml::initialize("nStarts", "", "Number of starts for multistart searchs", 3,
            DoubleRange(1, 16, 1), 0, root);
}

int VNNclReconstructionService::getMethodID(QDomElement root)
{
    return find(mMethods.begin(), mMethods.end(), this->getMethodOption(root)->getValue()) - mMethods.begin();
}

int VNNclReconstructionService::getPlaneMethodID(QDomElement root)
{
    return find(mPlaneMethods.begin(), mPlaneMethods.end(), this->getPlaneMethodOption(root)->getValue())
            - mPlaneMethods.begin();
}


} /* namespace cx */
