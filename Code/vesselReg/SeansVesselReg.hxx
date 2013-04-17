#ifndef SeansVesselSegmentation_hxx
#define SeansVesselSegmentation_hxx

#include "sscForwardDeclarations.h"
#include "vtkForwardDeclarations.h"
#include "sscTransform3D.h"
#include "vtkSmartPointer.h"

namespace ssc
{
/** Vessel - vessel registration algorithm.
 *
 * Input is two centerline representations of vessel trees.
 * Output is either a linear transform or a nonlinear transform (nonlinear not used yet).
 *
 * Basic usage: Run execute(), then get result with getLinearTransform()
 *
 *
 * \date Feb 4, 2011
 * \author Christian Askeland, SINTEF
 *
 * Original authors:
 *  Ingerid Reinertsen - algorithm.
 *  Sean (get full name) - refactored code.
 *
 */
class SeansVesselReg
{
public:
	/**Helper for storing all running data
	 * related to the v2v algorithm in one place.
	 */
	struct Context
	{
		vtkCellLocatorPtr mTargetPointLocator; ///< input: target data wrapped in a locator
		vtkPolyDataPtr mTargetPoints; ///< input: target data
		vtkPointsPtr mSourcePoints; ///< input: current source data, modified according to last iteration

		vtkPolyDataPtr getMovingPoints(); ///< the moving data (one of target or source, depending on inversion)
		vtkPolyDataPtr getFixedPoints(); ///< the fixed data (one of target or source, depending on inversion)

		vtkPointsPtr mSortedSourcePoints; ///< source points sorted according to distance to target, #mSortedSourcePoints==#mSortedTargetPoints
		vtkPointsPtr mSortedTargetPoints; ///< source points projected onto the target points (closest points) #mSortedSourcePoints==#mSortedTargetPoints

		vtkGeneralTransformPtr mConcatenation; ///< output: concatenation of all transforms so far
		vtkAbstractTransformPtr mTransform; ///< output: transform from last iteration
		double mMetric; ///< output: mean least squares from BEFORE last iteration.

		double mLtsRatio; ///< local copy of the lts ratio, can be changed for current iteration.

		//---------------------------------------------------------------------------
		//TODO non-linear needs to handle this!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		bool mInvertedTransform; ///< the calculated registration goes from target to source instead of source to target
		//---------------------------------------------------------------------------
	};
	typedef boost::shared_ptr<Context> ContextPtr;

	SeansVesselReg();
//	SeansVesselReg(int lts_ratio, double stop_delta, double lambda, double sigma, bool lin_flag, int sample,
//		int single_point_thre, bool verbose);
	~SeansVesselReg();

	bool execute(ssc::DataPtr source, ssc::DataPtr target, QString logPath);
	ssc::Transform3D getLinearResult(ContextPtr context=ContextPtr());
	double getResultMetric(ContextPtr context=ContextPtr());
	double getResultLtsRatio(ContextPtr context=ContextPtr());
	ssc::Transform3D getNonLinearTransform();
	void checkQuality(ssc::Transform3D linearTransform);
	ssc::ImagePtr loadMinc(char* source_file);
	void setDebugOutput(bool on)
	{
		mt_verbose = on;
	}

	bool mt_auto_lts;
	int mt_ltsRatio;
	double mt_distanceDeltaStopThreshold;
	double mt_lambda;
	double mt_sigma;
	bool mt_doOnlyLinear;
//	bool mt_doStepping;
	int mt_sampleRatio;
	int mt_singlePointThreshold;
	int mt_maximumNumberOfIterations;
	bool mt_verbose;
	double mt_maximumDurationSeconds;

	// debug interface:
	ContextPtr createContext(ssc::DataPtr source, ssc::DataPtr target);
	void performOneRegistration(ContextPtr context, bool linear);
	void computeDistances(ContextPtr context);
	static vtkPolyDataPtr convertToPolyData(vtkPointsPtr input);

	/**
	 * Extract polydata from a image.
	 * @param image
	 * @param p_neighborhoodFilterThreshold
	 * @param p_BoundingBox
	 * @return
	 */
	static vtkPolyDataPtr extractPolyData(ssc::ImagePtr image, int p_neighborhoodFilterThreshold,
		double p_BoundingBox[6]);
private:
	ssc::Transform3D getLinearTransform(vtkGeneralTransformPtr myConcatenation);

protected:
	bool runAlgorithm(ContextPtr context, vtkGeneralTransformPtr myConcatenation, int largeSteps, double fraction);
	void printOutResults(QString fileNamePrefix, vtkGeneralTransformPtr myConcatenation);
	vtkAbstractTransformPtr linearRegistration(vtkPointsPtr sortedSourcePoints, vtkPointsPtr sortedTargetPoints);
	vtkAbstractTransformPtr nonLinearRegistration(vtkPointsPtr sortedSourcePoints, vtkPointsPtr sortedTargetPoints);
	vtkPolyDataPtr convertToPolyData(ssc::DataPtr data);
	vtkPointsPtr transformPoints(vtkPointsPtr input, vtkAbstractTransformPtr transform);
	vtkPointsPtr createSortedPoints(vtkIdListPtr sortedIDList, vtkPointsPtr unsortedPoints, int numPoints);
	vtkPolyDataPtr crop(vtkPolyDataPtr input, vtkPolyDataPtr fixed, double margin);
	ContextPtr linearRefineAllLTS(ContextPtr context);
	void linearRefine(ContextPtr context);
	SeansVesselReg::ContextPtr splitContext(ContextPtr context);

	void print(vtkPointsPtr points);
	void print(vtkPolyDataPtr data);



//	ssc::Transform3D mLinearTransformResult;
	ContextPtr mLastRun; ///< result from last run of execute()

//	//---------------------------------------------------------------------------
//	//TODO non-linear needs to handle this!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//	bool mInvertedTransform; ///< the calculated registration goes from target to source instead of source to target
//	//---------------------------------------------------------------------------
};
}//namespace cx
#endif
