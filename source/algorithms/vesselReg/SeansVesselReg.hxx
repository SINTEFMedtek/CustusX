#ifndef SeansVesselSegmentation_hxx
#define SeansVesselSegmentation_hxx

#include "sscForwardDeclarations.h"
#include "sscTransform3D.h"
#include "vtkSmartPointer.h"

typedef vtkSmartPointer<class vtkPoints> vtkPointsPtr;
typedef vtkSmartPointer<class vtkPolyData> vtkPolyDataPtr;
typedef vtkSmartPointer<class vtkTransform> vtkTransformPtr;
typedef vtkSmartPointer<class vtkGeneralTransform> vtkGeneralTransformPtr;
typedef vtkSmartPointer<class vtkCellLocator> vtkCellLocatorPtr;
typedef vtkSmartPointer<class vtkMaskPoints> vtkMaskPointsPtr;
typedef vtkSmartPointer<class vtkThinPlateSplineTransform> vtkThinPlateSplineTransformPtr;
//typedef vtkSmartPointer<class HackTPSTransform> HackTPSTransformPtr;
typedef vtkSmartPointer<class vtkLandmarkTransform> vtkLandmarkTransformPtr;
typedef vtkSmartPointer<class vtkIdList> vtkIdListPtr;
typedef vtkSmartPointer<class vtkSortDataArray> vtkSortDataArrayPtr;
typedef vtkSmartPointer<class vtkFloatArray> vtkFloatArrayPtr;
typedef vtkSmartPointer<class vtkDataArray> vtkDataArrayPtr;
typedef vtkSmartPointer<class vtkCellArray> vtkCellArrayPtr;
typedef vtkSmartPointer<class vtkMINCImageReader> vtkMINCImageReaderPtr;
typedef vtkSmartPointer<class vtkAbstractTransform> vtkAbstractTransformPtr;

class vtkCellArray;
class vtkPoints;
class vtkAbstractTransform;

namespace cx
{
/** Vessel - vessel registration algorithm.
 *
 * Input is two centerline representations of vessel trees.
 * Output is either a linear transform or a nonlinear transform (nonlinear not used yet).
 *
 * Original authors:
 *  Ingerid Reinertsen - algorithm.
 *  Sean (get full name) - refactored code.
 *
 */
class SeansVesselReg
{
public:
  SeansVesselReg(int lts_ratio, double stop_delta, double lambda, double sigma, bool lin_flag, int sample, int single_point_thre, bool verbose);
  ~SeansVesselReg();

  void doItRight(ssc::ImagePtr source, ssc::ImagePtr target);
  ssc::Transform3D getLinearTransform();
//  ssc::Transform3D getNonLinearTransform();
  ssc::ImagePtr loadMinc(char* source_file);

private:
  vtkPolyDataPtr extractPolyData(ssc::ImagePtr image, int p_neighborhoodFilterThreshold, double p_BoundingBox[6]);
  ssc::Transform3D getLinearTransform(vtkGeneralTransformPtr myConcatenation);

protected:
  void processAllStuff(vtkPolyDataPtr currentSourcePolyData, vtkCellLocatorPtr myLocator, vtkGeneralTransformPtr myConcatenation);
  void printOutResults(char* fileNamePrefix, vtkGeneralTransformPtr myConcatenation);
  vtkAbstractTransformPtr linearRegistration(vtkPointsPtr sortedSourcePoints, vtkPointsPtr sortedTargetPoints, int numPoints/*, vtkAbstractTransform** myCurrentTransform*/);
  vtkAbstractTransformPtr nonLinearRegistration(vtkPolyDataPtr tpsSourcePolyData, vtkPolyDataPtr tpsTargetPolyData, int numPoints);

  int mt_ltsRatio;
  double mt_distanceDeltaStopThreshold;
  double mt_lambda;
  double mt_sigma;
  bool mt_doOnlyLinear;
  bool mt_doStepping;
  int mt_sampleRatio;
  int mt_singlePointThreshold;
  int mt_maximumNumberOfIterations;
  bool mt_verbose;

  ssc::Transform3D mLinearTransformResult;
};
}//namespace cx
#endif
