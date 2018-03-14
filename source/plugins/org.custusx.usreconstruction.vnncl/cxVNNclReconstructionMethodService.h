/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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

