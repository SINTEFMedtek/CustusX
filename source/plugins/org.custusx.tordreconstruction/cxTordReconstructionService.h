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

#ifndef CXTORDRECONSTRUCTIONSERVICE_H_
#define CXTORDRECONSTRUCTIONSERVICE_H_

#include "cxReconstructionService.h"
#include "org_custusx_tordreconstruction_Export.h"

#include "cxReconstructionService.h"
#include "cxUSFrameData.h"
#include "cxStringDataAdapterXml.h"
#include "cxDoubleDataAdapterXml.h"
#include "cxTordAlgorithm.h"


namespace cx
{

/**
 * Implementation of Tords reconstruction service.
 *
 * \ingroup org_custusx_tordreconstruction
 *
 * \date 2014-05-09
 * \author Janne Beate Bakeng
 */
class org_custusx_tordreconstruction_EXPORT TordReconstructionService : public ReconstructionService
{
	Q_INTERFACES(cx::ReconstructionService)
public:
    TordReconstructionService();
	virtual ~TordReconstructionService();

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
     * Return a set of DataAdapters describing the algorithm settings.
     * @param root The root element of the settings for this algorithm
     * @return A vector containing all the settings for this algorithm
     */
    virtual std::vector<DataAdapterPtr> getSettings(QDomElement root);

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
    virtual StringDataAdapterXmlPtr getMethodOption(QDomElement root);

    /**
     * Make radius option for the UI
     * @param root The root of the configuration ui
     * @return Radius data adapter - with selected value available by ->getValue()
     */
    virtual DoubleDataAdapterXmlPtr getRadiusOption(QDomElement root);


    /**
     * Make plane method option for the UI
     * @param root The root of the configuration ui
     * @return List of available methods - with the selected one available by ->getValue()
     */
    virtual StringDataAdapterXmlPtr getPlaneMethodOption(QDomElement root);

    /**
     * Make max planes option for the UI
     * @param root The root of the configuration ui
     * @return List of available methods - with the selected one available by ->getValue()
     */
    virtual DoubleDataAdapterXmlPtr getMaxPlanesOption(QDomElement root);

    /**
     * Make number of starts for multistart search option for the UI
     * @param root The root of the configuration ui
     * @return Number of multistart search starts option
     */
    virtual DoubleDataAdapterXmlPtr getNStartsOption(QDomElement root);

    /**
     * Make brightness weight(anisotropic method only) option for the UI
     * @param root The root of the configuration ui
     * @return Number of multistart search starts option
     */
    virtual DoubleDataAdapterXmlPtr getBrightnessWeightOption(QDomElement root);

        /**
     * Make Newness weight(anisotropic method only) option for the UI
     * @param root The root of the configuration ui
     * @return Number of multistart search starts option
     */
    virtual DoubleDataAdapterXmlPtr getNewnessWeightOption(QDomElement root);

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

    TordAlgorithmPtr mAlgorithm;


};
typedef boost::shared_ptr<TordReconstructionService> TordReconstructionServicePtr;

} /* namespace cx */

#endif /* CXTORDRECONSTRUCTIONSERVICE_H_ */

