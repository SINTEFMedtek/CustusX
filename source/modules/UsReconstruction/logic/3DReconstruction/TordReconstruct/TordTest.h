	// This is a preliminary test written by Tord Øygard in order to familiarize himself with the CustusX codebase.
#ifndef TORD_TEST_H_
#define TORD_TEST_H_

#include "cxReconstructAlgorithm.h"
#include "cxUSFrameData.h"
#include "cxStringDataAdapterXml.h"
#include "cxDoubleDataAdapterXml.h"
#include "cxTordAlgorithm.h"

namespace cx
{

/**
 * \addtogroup cx_module_usreconstruction
 * \{
 */


/**
 * \brief OpenCL Implementation of three 3D Reconstruction algorithms
 *
 * This class contains the OpenCL boilerplate code necessary for using
 * the TordTest set of reconstruction algorithms.
 * As seen from CustusX, this is one algorithm - but the actual algorithm used
 * is selectable by the DataAdapter given by getMethodOption() -
 * which is also included in getSettings().
 * The algorithms share a lot of common code, both CL boilerplate code as well as
 * actual OpenCL kernel code.
 */
class TordTest : public ReconstructAlgorithm
{
public:
	TordTest();
	virtual ~TordTest();

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

} //namespace cx

#endif
