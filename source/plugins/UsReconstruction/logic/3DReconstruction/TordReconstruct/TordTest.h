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
 * \brief Tords US test
 */

class TordTest : public ReconstructAlgorithm
{
public:
	TordTest();
	virtual ~TordTest();
	virtual QString getName() const
	{
		return "TordTest";
	}
	
	virtual std::vector<DataAdapterPtr> getSettings(QDomElement root);
	virtual bool reconstruct(ProcessedUSInputDataPtr input,
	                         vtkImageDataPtr outputData,
	                         QDomElement settings);

	typedef struct __frameBlock_t
	{
		unsigned char* data;
		size_t length;
	} frameBlock_t;

protected:
	
	virtual bool initCL(QString kernelFile,
	                    int nMaxPlanes,
	                    int nPlanes,
	                    int method,
	                    int planeMethod);
	
	virtual cl_program buildCLProgram(const char* program_src, 
	                                  int nMaxPlanes,
	                                  int nPlanes,
	                                  int method,
	                                  int planeMethod,
	                                  QString kernelPath);
	
	virtual bool doGPUReconstruct(ProcessedUSInputDataPtr input,
	                              vtkImageDataPtr outputData,
	                              float radius);

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
	virtual void freeFrameBlocks(frameBlock_t* framePointers,
	                             int numBlock);

	/**
	 * Retrieve the plane equations into the array planeEqs.
	 * The plane equations look like this: ax + by + cz + d = 0
	 * This is represented by 4 floats for each plane, stored like this:
	 * planeEqs[] = [ a0, b0, c0, d0, a1, b1, c1, d1, ... an, bn, cn, dn]
	 * where n = nPlanes.
	 * \param[out] planeEqs Pointer to float array of nPlanes*4
	 *                      elements to store plane equations in
	 * \param[in] input The US input to extract plane equations from
	 */	 
	virtual void fillPlaneEqs(float *planeEqs,
	                          ProcessedUSInputDataPtr input);

	/**
	 * Retrieve the positions of the plane corners in 3d space and store them in planeCorners.
	 * Each plane is stored with 3 corners (the 4th corner is implicitly defined by these),
	 * and each corner is a 3d point.
	 * The three stored corners are:
	 *  Corner 1: (0,0) (Lower left)
	 *  Corner 2: (x,0) (Lower right)
	 *  Corner 3: (0,y) (Upper left)
	 * This yields 9 values per plane, which are stored like this
	 * planeCorners[] = [p0c1x, p0c1y, p0c1z, p0c2x, p0c2y, p0c2z, p0c3x, p0c3y, p0c3z,
	 *                   p1c1x, p1c1y, p1c1z, p1c2x, p1c2y, p1c2z, p1c3x, p1c3y, p1c3z, ...]
	 * where pXcYk = coordinate K of corner Y of plane X.
	 * \param[out] planeCorners Pointer to float array of nPlanes*9 elements to store corners in
	 * \param[in] input The US input to extract plane corners from
	 */
	virtual void fillPlaneCorners(float *planeCorners,
	                              ProcessedUSInputDataPtr input);

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
	 * Make plane method option for the UI
	 * @param root The root of the configuration ui
	 * @return List of available methods - with the selected one available by ->getValue()
	 */
	virtual DoubleDataAdapterXmlPtr getMaxPlanesOption(QDomElement root);

	
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
