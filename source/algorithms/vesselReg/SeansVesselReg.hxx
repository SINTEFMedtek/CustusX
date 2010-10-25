#ifndef SeansVesselSegmentation_hxx
#define SeansVesselSegmentation_hxx

#include "sscForwardDeclarations.h"
#include "sscTransform3D.h"
#include "vtkSmartPointer.h"
//class UserInterfaceThingy;
typedef vtkSmartPointer<class vtkPolyData> vtkPolyDataPtr;
typedef vtkSmartPointer<class vtkTransform> vtkTransformPtr;


typedef vtkSmartPointer<class vtkGeneralTransform> vtkGeneralTransformPtr;
typedef vtkSmartPointer<class vtkCellLocator> vtkCellLocatorPtr;

class vtkCellArray;
class vtkPoints;
//class vtkPolyData;
//class vtkCellLocator;
class vtkAbstractTransform;
//class vtkGeneralTransform;

class SeansVesselReg
{
public:

  SeansVesselReg(int lts_ratio, double stop_delta, double lambda, double sigma, bool lin_flag, int sample,
      int single_point_thre, bool verbose);

  virtual ~SeansVesselReg();

  void doIt(char* source_file, char* target_file, char* source_landmarks, char* target_landmarks);

  ssc::Transform3D doItRight(ssc::ImagePtr source, ssc::ImagePtr target);
  ssc::ImagePtr loadMinc(char* source_file);

private:
  vtkPolyDataPtr extractPolyData(ssc::ImagePtr image, int p_neighborhoodFilterThreshold, double p_BoundingBox[6]);
  ssc::Transform3D getLinearTransform(vtkGeneralTransform* myConcatenation);

protected:

  void getSomeData(char *dataFile, int p_neighborhoodFilterThreshold, double p_BoundingBox[6],
      vtkPolyData *p_thePolyData);

  void processAllStuff(vtkPolyData* currentSourcePolyData, vtkCellLocator* myLocator,
      vtkGeneralTransform* myConcatenation);

  void weirdStuff2(char* source_landmarks, char* target_landmarks, vtkPolyData* corr_target_poly);

  void printOutResults(char* fileNamePrefix, vtkGeneralTransform* myConcatenation);

  void linearRegistration(vtkPoints *points1, vtkPoints *points2, int n_points,
      vtkAbstractTransform** myCurrentTransform);

  void nonLinearRegistration(vtkPolyData *tps_source_poly, vtkPolyData *tps_target_poly, int n_points,
      vtkAbstractTransform** myCurrentTransform);

  //    UserInterfaceThingy* mt_uIThingy;
  //
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
};

#endif
