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

#ifndef CXVNNCLRECONSTRUCTIONMETHODSERVICE_H_
#define CXVNNCLRECONSTRUCTIONMETHODSERVICE_H_

#include "org_custusx_usreconstruction_vnncl_Export.h"

#include "cxReconstructionMethodService.h"
#include "cxUSFrameData.h"
#include "cxStringProperty.h"
#include "cxDoubleProperty.h"
#include "cxVNNclAlgorithm.h"
class ctkPluginContext;


namespace cx
{
/**
 *  Original author Tord Øygard
 */

/**
 * Implementation of Tord Øygards reconstruction service.
 *
 * \ingroup org_custusx_vnnclreconstruction
 *
 * \date 2014-05-09
 * \author Janne Beate Bakeng
 */
class org_custusx_usreconstruction_vnncl_EXPORT VNNclReconstructionMethodService : public ReconstructionMethodService
{
	Q_INTERFACES(cx::ReconstructionMethodService)
public:
	VNNclReconstructionMethodService(ctkPluginContext* context);
	virtual ~VNNclReconstructionMethodService();

    /**
     * Turn OpenCL profiling on
     */
    void enableProfiling();

    double getKernelExecutionTime();

    /**
     * Return the name of the algorithm
     */
    virtual QString getName() const;

    /**
	 * Return a set of Properties describing the algorithm settings.
     * @param root The root element of the settings for this algorithm
     * @return A vector containing all the settings for this algorithm
     */
    virtual std::vector<PropertyPtr> getSettings(QDomElement root);

    /**
     * Reconstruction entry point.
     * @param input The processed input US data
     * @param outputData The volume will be stored here
     * @param settings The selected algorithms settings
     */
    virtual bool reconstruct(ProcessedUSInputDataPtr input,
                             vtkImageDataPtr outputData,
                             QDomElement settings);

    /**
     * Make method option for the UI
     * @param root The root of the configuration ui
     * @return List of available methods - with the selected one available by ->getValue()
     */
    virtual StringPropertyPtr getMethodOption(QDomElement root);

    /**
     * Make radius option for the UI
     * @param root The root of the configuration ui
     * @return Radius data adapter - with selected value available by ->getValue()
     */
    virtual DoublePropertyPtr getRadiusOption(QDomElement root);


    /**
     * Make plane method option for the UI
     * @param root The root of the configuration ui
     * @return List of available methods - with the selected one available by ->getValue()
     */
    virtual StringPropertyPtr getPlaneMethodOption(QDomElement root);

    /**
     * Make max planes option for the UI
     * @param root The root of the configuration ui
     * @return List of available methods - with the selected one available by ->getValue()
     */
    virtual DoublePropertyPtr getMaxPlanesOption(QDomElement root);

    /**
     * Make number of starts for multistart search option for the UI
     * @param root The root of the configuration ui
     * @return Number of multistart search starts option
     */
    virtual DoublePropertyPtr getNStartsOption(QDomElement root);

    /**
     * Make brightness weight(anisotropic method only) option for the UI
     * @param root The root of the configuration ui
     * @return Number of multistart search starts option
     */
    virtual DoublePropertyPtr getBrightnessWeightOption(QDomElement root);

        /**
     * Make Newness weight(anisotropic method only) option for the UI
     * @param root The root of the configuration ui
     * @return Number of multistart search starts option
     */
    virtual DoublePropertyPtr getNewnessWeightOption(QDomElement root);

protected:

    /**
     * Retrieve the method ID from the settings
     * @param root The algorithm settings from the UI
     * @return the method ID to use in the OpenCL kernel
     */
    virtual int getMethodID(QDomElement root);


    /**
     * Retrieve the plane method ID from the settings
     * @param root The algorithm settings from the UI
     * @return the plane method ID to use in the OpenCL kernel
     */
    virtual int getPlaneMethodID(QDomElement root);

    // Method names. Indices into this array corresponds to method IDs in the OpenCL Kernel.
    std::vector<QString> mMethods;
    std::vector<QString> mPlaneMethods;

    VNNclAlgorithmPtr mAlgorithm;
};

} /* namespace cx */

#endif /* CXVNNCLRECONSTRUCTIONMETHODSERVICE_H_ */

