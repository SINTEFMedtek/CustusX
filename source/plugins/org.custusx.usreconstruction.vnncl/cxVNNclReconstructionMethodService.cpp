/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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
	return "vnn_cl";
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
