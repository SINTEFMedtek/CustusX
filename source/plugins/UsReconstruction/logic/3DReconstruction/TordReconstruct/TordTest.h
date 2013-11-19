// This is a preliminary test written by Tord Øygard in order to familiarize himself with the CustusX codebase.
#ifndef TORD_TEST_H_
#define TORD_TEST_H_


#include "sscReconstructAlgorithm.h"
#include "Thunder/setup.h"
#include <sscUSFrameData.h>
#include "sscStringDataAdapterXml.h"
#include "sscDoubleDataAdapterXml.h"


namespace cx
{
/**
 * \addtogroup sscUSReconstruction
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
	 * Return the name of the algorithm
	 */
	virtual QString getName() const
	{
		return "TordTest";
	}
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

protected:

	/**
	 * A representation of a Frame block in CPU memory.
	 * Since OpenCL has an upper limit on how big memory objects can be, we need to
	 * partition the input US images into several blocks. This struct represents
	 * one such block
	 */
	typedef struct __frameBlock_t
	{
		unsigned char* data;
		size_t length;
	} frameBlock_t;

	/**
	 * Initialize OpenCL.
	 * This function initializes OpenCL and builds the kernel with the given parameters.
	 * Note that these parameters are set at compile time and not kernel run time.
	 * Uses buildCLProgram() to build the kernel.
	 * @param kernelFile Path to the OpenCL kernel source
	 * @param nMaxPlanes The MAX_PLANES parameter of the kernel, i.e. max value of how many planes to include in the reconstruction of one voxel
	 * @param nPlanes Number of image planes in the input data set
	 * @param method The method ID. See kernels.ocl for more information
	 * @param planeMethod the plane method ID. See kernels.ocl for more information
	 * @return True on success
	 * @sa buildCLProgram
	 */
	virtual bool initCL(QString kernelFile,
	                    int nMaxPlanes,
	                    int nPlanes,
	                    int method,
	                    int planeMethod);
	
	/**
	 * Build the OpenCL kernel
	 * @param program_src The kernel source code
	 * @param nMaxPlanes The MAX_PLANES parameter of the kernel, i.e. max value of how many planes to include in the reconstruction of one voxel
	 * @param nPlanes Number of image planes in the input data set
	 * @param method The method ID. See kernels.ocl for more information
	 * @param planeMethod the plane method ID. See kernels.ocl for more information
	 * @param kernelPath The path of the kernel source code
	 * @return True on suc
	 */
	virtual cl_program buildCLProgram(const char* program_src, 
	                                  int nMaxPlanes,
	                                  int nPlanes,
	                                  int method,
	                                  int planeMethod,
	                                  QString kernelPath);
	/**
	 * Perform GPU Reconstruction.
	 * This function initializes the CL memory objects, calls the kernel and reads back the result,
	 * which is stored in outputData. 
	 * @param input The input US data
	 * @param outputData The output volume is stored here
	 * @param radius The radius of the kernel - i.e. how far away to look for image planes to use
	 * @param nClosePlanes The number of close planes to search for
	 * @return true on success
	 */	 
	virtual bool doGPUReconstruct(ProcessedUSInputDataPtr input,
	                              vtkImageDataPtr outputData,
	                              float radius,
	                              int nClosePlanes);

	/**
	 * Split the US input into numBlock blocks of whole frames 
	 * and store them in framePointers.
	 * \param[out] framePointers Pre-allocated array of numBlocks unsigned char* pointers
	 *             Data will be returned here
	 * \param[in] numBlocks Number of blocks to split US input into
	 * \param[in] inputFrames The input US B-scans
	 * \return true on success, false otherwise
	 */
	virtual bool initializeFrameBlocks(frameBlock_t* framePointers,
	                                   int numBlocks,
	                                   ProcessedUSInputDataPtr inputFrames);
	/**
	 * Free a set of frame blocks allocated by initializeFrameBlocks
	 * Does not free the actual frameBlock_t structure, but the data pointer is free'd.
	 * You still have to free the frameBlocks!
	 * @param framePointers Pointer to first element in array of frame blocks
	 * @param numBlocks number of blocks in the array
	 */
	virtual void freeFrameBlocks(frameBlock_t* framePointers,
	                             int numBlock);
	

	/**
	 * Fill the supplied array of floats with the values from the transformation matrices
	 * in input.
	 * The data is stored like this:
	 *  [ a b c d
	 *    e f g h    -> [ a b c d e f g h i j k l m n o p ]
	 *    i j k l
	 *    m n o p ]
	 * I.e: 16 floats per image plane, row major.
	 * @param planeMatrices Pointer to array of 16*number of image planes
	 *                       floats where the matrices will be stored
	 * @param input The US data - which also has position data
	 */
	virtual void fillPlaneMatrices(float *planeMatrices,
	                               ProcessedUSInputDataPtr input);

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

	/// OpenCL handles
	cl_kernel mClKernel;
	std::vector<cl_mem> mVClMemBscan;
	cl_mem mClMemOutput;
	ocl_context* moClContext;

	// Method names. Indices into this array corresponds to method IDs in the OpenCL Kernel.
	std::vector<QString> mMethods;
	std::vector<QString> mPlaneMethods;
	
};


}
		
#endif
