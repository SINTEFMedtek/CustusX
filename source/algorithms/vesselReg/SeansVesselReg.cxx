#include <iostream>
#include <time.h>
//#include <bits/time.h>

#include "SeansVesselReg.hxx"
#include "sscImage.h"
#include "sscTypeConversions.h"
#include "sscRegistrationTransform.h"
//#include "UserInterfaceThingy.hxx"
#include "HackTPSTransform.hxx"

#include <time.h>
#include "vtkPoints.h"
#include "vtkPolyData.h"
#include "vtkCellArray.h"
#include "vtkCellLocator.h"
#include "vtkMINCImageReader.h"
#include "vtkTransform.h"
#include "vtkImageData.h"

#include "vtkGeneralTransform.h"
#include "vtkMath.h"
#include "vtkSortDataArray.h"
#include "vtkMaskPoints.h"
#include "vtkPointData.h"
#include "vtkLandmarkTransform.h"
#include "vtkFloatArray.h"
#include <fstream> 

SeansVesselReg::SeansVesselReg(int lts_ratio, double stop_delta, double lambda, double sigma, bool lin_flag,
    int sample, int single_point_thre, bool verbose) :
  mt_ltsRatio(lts_ratio), mt_distanceDeltaStopThreshold(stop_delta), mt_lambda(lambda), mt_sigma(sigma),
      mt_doOnlyLinear(lin_flag), mt_sampleRatio(sample), mt_singlePointThreshold(single_point_thre),
      //    mt_uIThingy(0),
      mt_maximumNumberOfIterations(100), mt_verbose(verbose)
{
}

SeansVesselReg::~SeansVesselReg()
{

}

void SeansVesselReg::getSomeData(char *p_dataFile, int p_neighborhoodFilterThreshold, double p_BoundingBox[6],
    vtkPolyData *p_thePolyData)
{
  vtkPolyDataPtr targetPolyData = vtkPolyDataPtr::New();

  time_t sec1 = clock();
  std::cout << "Reading " << p_dataFile << " -> ";
  std::cout.flush();

  //Read data input file
  vtkMINCImageReader *l_dataReader = vtkMINCImageReader::New();
  l_dataReader->SetFileName(p_dataFile);
  l_dataReader->Update();

  double l_dataOrigin[3];
  l_dataReader->GetOutput()->GetOrigin(l_dataOrigin);
  std::cout << (clock() - sec1) / (double) CLOCKS_PER_SEC << " secs...DONE -> Processing...";
  std::cout.flush();
  int l_dimensions[3];
  l_dataReader->GetOutput()->GetDimensions(l_dimensions);

  //set the transform
  vtkTransform *l_dataTransform = vtkTransform::New();
  l_dataTransform->SetMatrix(l_dataReader->GetDirectionCosines());
  l_dataTransform->Translate(l_dataReader->GetDataOrigin());
  l_dataTransform->GetInverse()->TransformPoint(l_dataOrigin, l_dataOrigin);
  l_dataTransform->Translate(l_dataOrigin);
  l_dataTransform->Scale(l_dataReader->GetOutput()->GetSpacing());
  int l_startPosX, l_startPosY, l_startPosZ; //Beginings of neighborhood offsets
  int l_stopPosX, l_stopPosY, l_stopPosZ; //Ends of neighborhood offsets
  int i, j, k, ii, jj, kk, l_counts = 0; //Counter variables

  bool l_isNeighborFound; //Boolean for loop breakout

  //dimensions, fast if we first deref it in to variables
  int l_dimX = l_dimensions[0];
  int l_dimY = l_dimensions[1];
  int l_dimZ = l_dimensions[2];

  double* l_tempPoint;
  //Offsets variables
  int l_offsetI = 0;
  int l_offsetJ = 0;
  int l_offsetK = 0;
  int l_kkjjOffset = 0;
  int l_kkjjiiOffset = 0;
  int l_offsetConstIJ = l_dimX * l_dimY;

  vtkDataArray* l_allTheData = l_dataReader->GetOutput()->GetPointData()->GetScalars();
  vtkPoints *l_dataPoints = vtkPoints::New();
  vtkCellArray *l_dataCellArray = vtkCellArray::New();

  //Loop through the entire volume and precalculate the offsets for each
  //point along with the points neighborhood offset
  for (k = 0; k < l_dimZ; ++k)
  {
    //the start and stop offsets for the Z neighborhood (a gap in a cube)
    l_startPosZ = k - p_neighborhoodFilterThreshold;
    if (l_startPosZ < 0)
      l_startPosZ = 0;
    else
      l_startPosZ *= l_offsetConstIJ;

    l_stopPosZ = k + p_neighborhoodFilterThreshold;
    if (l_stopPosZ >= l_dimZ)
      l_stopPosZ = (l_dimZ - 1) * l_offsetConstIJ;
    else
      l_stopPosZ *= l_offsetConstIJ;

    l_offsetK = k * l_offsetConstIJ;

    for (j = 0; j < l_dimY; ++j)
    {
      //the start and stop offsets for the Y neighborhood (a hole through a cube)
      l_startPosY = j - p_neighborhoodFilterThreshold;
      if (l_startPosY < 0)
        l_startPosY = 0;
      else
        l_startPosY *= l_dimX;

      l_stopPosY = j + p_neighborhoodFilterThreshold;
      if (l_stopPosY >= l_dimY)
        l_stopPosY = (l_dimY - 1) * l_dimX;
      else
        l_stopPosY *= l_dimX;

      l_offsetJ = l_offsetK + (j * l_dimX);

      for (i = 0; i < l_dimX; ++i)
      {
        //the start and stop offsets for the X neighborhood (a voxel)
        l_startPosX = i - p_neighborhoodFilterThreshold;
        if (l_startPosX < 0)
          l_startPosX = 0;

        l_stopPosX = i + p_neighborhoodFilterThreshold;
        if (l_stopPosX >= l_dimX)
          l_stopPosX = l_dimX - 1;

        l_offsetI = l_offsetJ + i;
        l_isNeighborFound = 0;

        //See if this voxel contain a vessel center, if so do some more processing
        if (*(l_allTheData->GetTuple(l_offsetI)))
        {
          l_tempPoint = l_dataTransform->TransformPoint(i, j, k);

          //Do stuff if there is no bounding box, or if there is one check if the
          //point is in the bounding box
          if (!p_BoundingBox || (p_BoundingBox[0] < l_tempPoint[0] && p_BoundingBox[1] > l_tempPoint[0]
              && p_BoundingBox[2] < l_tempPoint[1] && p_BoundingBox[3] > l_tempPoint[1] && p_BoundingBox[4]
              < l_tempPoint[2] && p_BoundingBox[5] > l_tempPoint[2]))
          {
            //Loop through the neigbors of the point and see if they are vessel centers
            //if one of them is it then write the point down
            for (kk = l_startPosZ; kk <= l_stopPosZ && !l_isNeighborFound; kk += l_offsetConstIJ)
            {
              for (jj = l_startPosY; jj <= l_stopPosY && !l_isNeighborFound; jj += l_dimX)
              {
                l_kkjjOffset = kk + jj;

                for (ii = l_startPosX; ii <= l_stopPosX && !l_isNeighborFound; ++ii)
                {
                  l_kkjjiiOffset = ii + l_kkjjOffset;

                  if (l_offsetI != l_kkjjiiOffset && //ignore if it is the current point
                      *(l_allTheData->GetTuple(l_kkjjiiOffset))) //check if vessel center
                  {
                    l_isNeighborFound = 1;
                    l_dataPoints->InsertNextPoint(l_tempPoint);
                    l_dataCellArray->InsertNextCell(1);
                    l_dataCellArray->InsertCellPoint(l_counts++);
                  }
                }
              }
            }
          }
        }
      }
    }
  }

  p_thePolyData->SetPoints(l_dataPoints);
  p_thePolyData->SetVerts(l_dataCellArray);

  l_dataTransform->Delete();
  l_dataReader->Delete();

  std::cout << "DONE" << std::endl;
}

void SeansVesselReg::doIt(char* sourceFile, char* targetFile, char* outputTransform, char* targetLandmarks)
{
  std::cout << "stop Threshold:" << mt_distanceDeltaStopThreshold << endl;
  std::cout << "sigma:" << mt_sigma << endl;
  std::cout << "lts Ratio:" << mt_ltsRatio << endl;
  std::cout << "linear flag:" << mt_doOnlyLinear << endl;
  std::cout << "sample flag:" << mt_sampleRatio << endl;
  std::cout << "single Point Threshold:" << mt_singlePointThreshold << endl;

  time_t sec1 = clock();//time(NULL);

  //Grab the information from the files of target then
  //filter out points not fit for the threshold
  vtkPolyData *targetPolyData = vtkPolyData::New();
  getSomeData(targetFile, mt_singlePointThreshold, 0, targetPolyData);

  targetPolyData->GetPoints()->ComputeBounds();
  double* targetBounds = targetPolyData->GetPoints()->GetBounds();
  double searchAround = 40;
  targetBounds[0] -= searchAround;
  targetBounds[1] += searchAround;
  targetBounds[2] -= searchAround;
  targetBounds[3] += searchAround;
  targetBounds[4] -= searchAround;
  targetBounds[5] += searchAround;

  vtkPolyData *sourcePolyData = vtkPolyData::New();
  getSomeData(sourceFile, mt_singlePointThreshold, targetBounds, sourcePolyData);

  std::cout << "\nPreprocess time:" << " " << (clock() - sec1) / (double) CLOCKS_PER_SEC << " seconds\n" << endl;

  //Make sure we have stuff to work with
  if (!sourcePolyData->GetNumberOfPoints() || !targetPolyData->GetNumberOfPoints())
  {
    std::cerr << "Can't execute with empty source or target data" << std::endl;
    return;
  }

  vtkIdType numPoints = sourcePolyData->GetNumberOfPoints();
  std::cout << "total number of points:" << numPoints << endl;
  std::cout << "number of points to be sampled:" << ((int)(numPoints * mt_ltsRatio) / 100) << "\n" << endl;

  //Paint
  //    mt_uIThingy = new UserInterfaceThingy(sourcePolyData, targetPolyData);


  // Create locator for target points
  vtkCellLocator* targetPointLocator = vtkCellLocator::New();
  targetPointLocator->SetDataSet(targetPolyData);
  targetPointLocator->SetNumberOfCellsPerBucket(1);
  targetPointLocator->BuildLocator();

  //Container for all the transforms
  vtkGeneralTransform* myConcatenation = vtkGeneralTransform::New();

  //Do EVERYTHING
  processAllStuff(sourcePolyData, targetPointLocator, myConcatenation);

  printOutResults(outputTransform, myConcatenation);

  myConcatenation->Delete();

  std::cout << "\n\n\nExecution time:" << " " << (clock() - sec1) / (double) CLOCKS_PER_SEC << " " << "seconds" << endl;

  //    mt_uIThingy->render();
  //    mt_uIThingy->startInteraction();
  //
  //
  //    delete mt_uIThingy;

  targetPointLocator->Delete();
  sourcePolyData->Delete();
  targetPolyData->Delete();
}


void SeansVesselReg::printOutResults(char* fileNamePrefix, vtkGeneralTransform* myConcatenation)
{

  vtkMatrix4x4* l_tempMatrix = vtkMatrix4x4::New();
  vtkMatrix4x4* l_resultMatrix = vtkMatrix4x4::New();

  if (mt_doOnlyLinear)
    l_tempMatrix->DeepCopy(((vtkLandmarkTransform*) myConcatenation->GetConcatenatedTransform(0))->GetMatrix());

  l_resultMatrix->Identity();
  for (int i = 1; i < myConcatenation->GetNumberOfConcatenatedTransforms(); ++i)
  {
    vtkMatrix4x4::Multiply4x4(l_tempMatrix,
        ((vtkLandmarkTransform*) myConcatenation->GetConcatenatedTransform(i))->GetMatrix(), l_resultMatrix);
    l_tempMatrix->DeepCopy(l_resultMatrix);

  }
  std::cout << "oeuaoeu " << fileNamePrefix << std::endl;

  std::string nonLinearFile = fileNamePrefix;
  nonLinearFile += "--NonLinear";
  nonLinearFile += ".xfm";

  std::string linearFile = fileNamePrefix;
  linearFile += "--Linear";
  linearFile += ".xfm";

  std::cout << "Writing Results:" << std::endl;

  if (!mt_doOnlyLinear)
  {
    ofstream file_out(nonLinearFile.c_str());

    //Non-linear Warped Transform
    HackTPSTransform* l_theWarpTransform = ((HackTPSTransform*) myConcatenation->GetConcatenatedTransform(0));

    // Write the header
    file_out << "MNI Transform File\n" << std::endl;
    //file_out<<"SeansWarpyTransforms: source"<<std::endl;
    //file_out<<"SeansWarpyTransforms: target\n"<<std::endl;
    file_out << "Transform_Type = Thin_Plate_Spline_Transform;" << std::endl;
    file_out << "Invert_Flag = True;" << std::endl;
    file_out << "Number_Dimensions = 3;" << std::endl;
    int n = l_theWarpTransform->GetSourceLandmarks()->GetNumberOfPoints();

    const double* const * theWarpMatrix = l_theWarpTransform->GetWarpMatrix();
    double point[3];
    file_out << "Points = " << std::endl;
    for (int i = 0; i < n; i++)
    {
      l_theWarpTransform->GetSourceLandmarks()->GetPoint(i, point);
      file_out << point[0] << " " << point[1] << " " << point[2];
      if (i == n - 1)
        file_out << ";" << std::endl;
      else
        file_out << std::endl;
    }

    file_out << "Displacements = " << std::endl;
    for (int i = 0; i < n + 4; i++)
    {
      file_out << theWarpMatrix[i][0] << " " << theWarpMatrix[i][1] << " " << theWarpMatrix[i][2];
      if (i == n + 3)
        file_out << ";" << std::endl;
      else
        file_out << std::endl;
    }

    l_theWarpTransform->Delete();

    file_out.close();

  }

  ofstream file_out2(linearFile.c_str());

  //Linear Transform
  file_out2 << "MNI Transform File\n" << std::endl;
  //file_out2<<"SeansLinearTransforms: source"<<std::endl;
  //file_out2<<"SeansLinearTransforms: target\n"<<std::endl;
  file_out2 << "Transform_Type = Linear;" << std::endl;
  file_out2 << "Linear_Transform = ";
  file_out2 << std::endl;

  for (int i = 0; i < 3; i++)
  {
    for (int j = 0; j < 4; j++)
    {

      file_out2 << l_resultMatrix->GetElement(i, j);
      if (j != 3)
        file_out2 << " ";
    }
    if (i == 2)
      file_out2 << ";" << std::endl;
    else
      file_out2 << std::endl;
  }

  l_tempMatrix->Delete();
  l_resultMatrix->Delete();
  file_out2.close();
}

void SeansVesselReg::processAllStuff(vtkPolyData* currentSourcePolyData, vtkCellLocator* targetPointLocator,
    vtkGeneralTransform* myConcatenation)
{
  //Since we are going to play with the data, we have to make a copy
  vtkPoints* currentSourcePoints = vtkPoints::New();
  currentSourcePoints->DeepCopy(currentSourcePolyData->GetPoints());
  int numPoints = currentSourcePoints->GetNumberOfPoints();
  int nb_points = ((int)(numPoints * mt_ltsRatio) / 100);
  vtkIdType cell_id;
  int sub_id;
  double outPoint[3];

  // - closesetPoint is used so that the internal state of LandmarkTransform remains
  //   correct whenever the iteration process is stopped (hence its source
  //   and landmark points might be used in a vtkThinPlateSplineTransform).
  vtkPoints *closesetPoint = vtkPoints::New();
  closesetPoint->SetNumberOfPoints(numPoints);

  float mean_distance[mt_maximumNumberOfIterations];
  bool l_keepRunning = 1;
  double difference = 100;

  for (int myNumberOfIterations = 1; l_keepRunning && myNumberOfIterations < mt_maximumNumberOfIterations; ++myNumberOfIterations)
  {
    // Fill points with the closest points to each vertex in input
    vtkFloatArray *residuals = vtkFloatArray::New();
    residuals->SetNumberOfValues(numPoints);

    vtkIdList *IdList = vtkIdList::New();
    IdList->SetNumberOfIds(numPoints);
    double total_distance = 0;
    double distanceSquared = 0;
    double distance[numPoints];

    //Find closest points to all source points
    for (int i = 0; i < numPoints; ++i)
    {
      //Check the distance to neighbouring points (neighbours should be matched to nearby points)
      targetPointLocator->FindClosestPoint(currentSourcePoints->GetPoint(i), outPoint, cell_id, sub_id, distanceSquared);
      closesetPoint->SetPoint(i, outPoint);
      residuals->InsertValue(i, distanceSquared);
      IdList->InsertId(i, i);
      distance[i] = sqrt(distanceSquared);
      total_distance += distance[i];
    }
    mean_distance[myNumberOfIterations] = total_distance / numPoints;

    vtkSortDataArray *sort = vtkSortDataArray::New();
    sort->Sort(residuals, IdList);

    if (myNumberOfIterations != 1)
    {
      difference = mean_distance[myNumberOfIterations] - mean_distance[myNumberOfIterations - 1];
    }

    residuals->Delete();
    sort->Delete();

    vtkPoints *sortedSourcePoints = vtkPoints::New();
    sortedSourcePoints->SetNumberOfPoints(nb_points);

    vtkPoints *sortedTargetPoints = vtkPoints::New();
    sortedTargetPoints->SetNumberOfPoints(nb_points);

    double lts_point[3], lts_target_point[3];
    vtkIdType index;

    for (int i = 0; i < nb_points; ++i)
    {
      index = IdList->GetId(i);
      currentSourcePoints->GetPoint(index, lts_point); // source points to use in tps
      closesetPoint->GetPoint(index, lts_target_point); //target points to use in tps
      sortedSourcePoints->SetPoint(i, lts_point);
      sortedTargetPoints->SetPoint(i, lts_target_point);
    }

    IdList->Delete();

    vtkAbstractTransform* myCurrentTransform = 0;

    if (mt_doOnlyLinear)
    {
      linearRegistration(sortedSourcePoints, sortedTargetPoints, numPoints, &myCurrentTransform);
    }
    else
    {

      linearRegistration(sortedSourcePoints, sortedTargetPoints, numPoints, &myCurrentTransform);

      if (fabs(difference) < mt_distanceDeltaStopThreshold)
      {
        vtkCellArray * tps_sourceCellArray = vtkCellArray::New();
        vtkCellArray * tps_targetCellArray = vtkCellArray::New();

        for (int i = 0; i < nb_points; ++i)
        {

          tps_sourceCellArray->InsertNextCell(1);
          tps_sourceCellArray->InsertCellPoint(i);
          tps_targetCellArray->InsertNextCell(1);
          tps_targetCellArray->InsertCellPoint(i);
        }

        vtkPolyData *tps_source_poly = vtkPolyData::New();
        tps_source_poly->SetPoints(sortedSourcePoints);
        tps_source_poly->SetVerts(tps_sourceCellArray);

        vtkPolyData *tps_target_poly = vtkPolyData::New();
        tps_target_poly->SetPoints(sortedTargetPoints);
        tps_target_poly->SetVerts(tps_targetCellArray);

        nonLinearRegistration(tps_source_poly, tps_target_poly, numPoints, &myCurrentTransform);

        tps_sourceCellArray->Delete();
        tps_targetCellArray->Delete();
        tps_source_poly->Delete();
        tps_target_poly->Delete();
      }
    }

    sortedSourcePoints->Delete();
    sortedTargetPoints->Delete();

    vtkPoints *transformedSourcePoints = vtkPoints::New();
    transformedSourcePoints->SetNumberOfPoints(numPoints);

    //Transform ALL source points
    double tempPostTransPoint[3];
    for (int i = 0; i < numPoints; ++i)
    {
      myCurrentTransform->InternalTransformPoint(currentSourcePoints->GetPoint(i), tempPostTransPoint);
      transformedSourcePoints->SetPoint(i, tempPostTransPoint);
    }
    myConcatenation->Concatenate(myCurrentTransform);

    if (fabs(difference) < mt_distanceDeltaStopThreshold)
    {
      //Stop the running
      l_keepRunning = 0;

      vtkCellArray * tps_sourceCellArray = vtkCellArray::New();

      for (int i = 0; i < nb_points; ++i)
      {
        tps_sourceCellArray->InsertNextCell(1);
        tps_sourceCellArray->InsertCellPoint(i);
      }

      vtkPolyData *tps_source_poly = vtkPolyData::New();
      tps_source_poly->SetPoints(transformedSourcePoints);
      tps_source_poly->SetVerts(tps_sourceCellArray);

      //	    mt_uIThingy->addActorToRightRenderer(tps_source_poly, 0, 1, 0);

      tps_sourceCellArray->Delete();
      tps_source_poly->Delete();
    }

    vtkPoints* allTempPoints = currentSourcePoints;
    currentSourcePoints = transformedSourcePoints;
    transformedSourcePoints = allTempPoints;

    transformedSourcePoints->Delete();

    std::cout << myNumberOfIterations << " ";
    std::cout.flush();
  }

  closesetPoint->Delete();

  myConcatenation->Update();
}

void SeansVesselReg::linearRegistration(vtkPoints *sortedSourcePoints, vtkPoints *sortedTargetPoints, int numPoints,
    vtkAbstractTransform** myCurrentTransform)
{
  //Build landmark transform
  vtkLandmarkTransform *lmt = vtkLandmarkTransform::New();
  lmt->SetSourceLandmarks(sortedSourcePoints);
  lmt->SetTargetLandmarks(sortedTargetPoints);
  lmt->SetModeToRigidBody();
  lmt->Modified();
  lmt->Update();

  //     if(mt_verbose)
  //     {
  // 	double theLTSMatrix[4][4];

  // 	SeansHARP sharp;
  // 	sharp.leastTrimmedSquaresTransform(sortedSourcePoints,
  // 					   sortedTargetPoints,
  // 					   theLTSMatrix);
  // 	vtkMatrix4x4* theLTSMatrix2 = lmt->GetMatrix();

  // 	for(int i = 0; i < 4; i++ )
  // 	{
  // 	    if(theLTSMatrix[i][0] != theLTSMatrix2->GetElement(i,0) ||
  // 	       theLTSMatrix[i][1] != theLTSMatrix2->GetElement(i,1) ||
  // 	       theLTSMatrix[i][2] != theLTSMatrix2->GetElement(i,2) ||
  // 	       theLTSMatrix[i][3] != theLTSMatrix2->GetElement(i,3))
  // 		std::cout<<"Wrong at: "<<i<<std::endl;
  // 	    else
  // 	    {
  // 		std::cout<<" :) ";
  // 		std::cout.flush();
  // 	    }
  // 	}

  //     }
  *myCurrentTransform = lmt;
}

void SeansVesselReg::nonLinearRegistration(vtkPolyData *tpsSourcePolyData, vtkPolyData *tpsTargetPolyData,
    int numPoints, vtkAbstractTransform** myCurrentTransform)
{
  vtkMaskPoints *mask1 = vtkMaskPoints::New();
  mask1->SetInput(tpsSourcePolyData);
  mask1->SetOnRatio(mt_sampleRatio);
  mask1->Update();
  vtkMaskPoints *mask2 = vtkMaskPoints::New();
  mask2->SetInput(tpsTargetPolyData);
  mask2->SetOnRatio(mt_sampleRatio);
  mask2->Update();

  // Build the thin plate spline transform
  vtkThinPlateSplineTransform *tps = vtkThinPlateSplineTransform::New();
  tps->SetSourceLandmarks(mask1->GetOutput()->GetPoints());
  tps->SetTargetLandmarks(mask2->GetOutput()->GetPoints());
  tps->SetBasisToR();
  tps->SetSigma(mt_sigma);
  //    tps->Update();

  //    if(mt_verbose)
  //     {
  // 	numPoints = mask1->GetOutput()->GetPoints()->GetNumberOfPoints();

  // 	int rows = numPoints+3+1;
  // 	int cols = 3;
  // 	double* matrix = new double[rows*cols];
  // 	double** theWarpMatrix = new double *[rows];
  // 	for(int i = 0; i < rows; i++)
  // 	{
  // 	    theWarpMatrix[i] = &matrix[i*cols];
  // 	}
  // 	double theLambda = 0.0;

  // 	SeansHARP sharp;

  // 	sharp.thinPlateSplineWarpTrasform(mask1->GetOutput()->GetPoints(),
  // 					  mask2->GetOutput()->GetPoints(),
  // 					  mt_sigma,
  // 					  theLambda,
  // 					  theWarpMatrix);

  // 	const double* const* theWarpMatrix2 = ((HackTPSTransform*)tps)->GetWarpMatrix();

  // 	std::cout<<"Checking ..."<<std::endl;
  // 	for(int i = 0; i < numPoints+4; i++ )
  // 	{
  // 	    if(theWarpMatrix[i][0] != theWarpMatrix2[i][0] ||
  // 	       theWarpMatrix[i][1] != theWarpMatrix2[i][1] ||
  // 	       theWarpMatrix[i][2] != theWarpMatrix2[i][2])
  // 		std::cout<<"Wrong at: "<<i<<std::endl;
  // 	}

  // 	std::cout<<"Checking Done"<<std::endl;

  // 	delete [] *theWarpMatrix;
  // 	delete [] theWarpMatrix;
  //     }

  *myCurrentTransform = tps;

  mask1->Delete();
  mask2->Delete();
}

///--------------------------------------------------------
///--------------------------------------------------------
///--------------------------------------------------------

ssc::Transform3D SeansVesselReg::doItRight(ssc::ImagePtr source, ssc::ImagePtr target)
{
  std::cout << "stop Threshold:" << mt_distanceDeltaStopThreshold << endl;
  std::cout << "sigma:" << mt_sigma << endl;
  std::cout << "lts Ratio:" << mt_ltsRatio << endl;
  std::cout << "linear flag:" << mt_doOnlyLinear << endl;
  std::cout << "sample flag:" << mt_sampleRatio << endl;
  std::cout << "single Point Threshold:" << mt_singlePointThreshold << endl;

  time_t sec1 = clock();//time(NULL);

  //Grab the information from the files of target then
  //filter out points not fit for the threshold
//  vtkPolyData *targetPolyData = vtkPolyData::New();
//  getSomeData(targetFile, mt_singlePointThreshold, 0, targetPolyData);
  vtkPolyDataPtr targetPolyData = this->extractPolyData(target, mt_singlePointThreshold, 0);

  // use the bounding box of target to clip the source data.
  targetPolyData->GetPoints()->ComputeBounds();
  double* targetBounds = targetPolyData->GetPoints()->GetBounds();
  double searchAround = 40;
  targetBounds[0] -= searchAround;
  targetBounds[1] += searchAround;
  targetBounds[2] -= searchAround;
  targetBounds[3] += searchAround;
  targetBounds[4] -= searchAround;
  targetBounds[5] += searchAround;

//  vtkPolyData *sourcePolyData = vtkPolyData::New();
//  getSomeData(sourceFile, mt_singlePointThreshold, targetBounds, sourcePolyData);
  vtkPolyDataPtr sourcePolyData = this->extractPolyData(target, mt_singlePointThreshold, targetBounds);

  std::cout << "\nPreprocess time:" << " " << (clock() - sec1) / (double) CLOCKS_PER_SEC << " seconds\n" << endl;

  //Make sure we have stuff to work with
  if (!sourcePolyData->GetNumberOfPoints() || !targetPolyData->GetNumberOfPoints())
  {
    std::cerr << "Can't execute with empty source or target data" << std::endl;
    return ssc::Transform3D();
  }

  vtkIdType numPoints = sourcePolyData->GetNumberOfPoints();
  std::cout << "total number of points:" << numPoints << endl;
  std::cout << "number of points to be sampled:" << ((int)(numPoints * mt_ltsRatio) / 100) << "\n" << endl;

  //Paint
  //    mt_uIThingy = new UserInterfaceThingy(sourcePolyData, targetPolyData);


  // Create locator for target points
  vtkCellLocatorPtr targetPointLocator = vtkCellLocatorPtr::New();
  targetPointLocator->SetDataSet(targetPolyData);
  targetPointLocator->SetNumberOfCellsPerBucket(1);
  targetPointLocator->BuildLocator();

  //Container for all the transforms
  vtkGeneralTransformPtr myConcatenation = vtkGeneralTransformPtr::New();

  //Do EVERYTHING
  processAllStuff(sourcePolyData, targetPointLocator, myConcatenation);


  printOutResults(cstring_cast(source->getFilePath()+"/vessel_debug_"), myConcatenation);

//  myConcatenation->Delete();

  std::cout << "\n\n\nExecution time:" << " " << (clock() - sec1) / (double) CLOCKS_PER_SEC << " " << "seconds" << endl;

  //    mt_uIThingy->render();
  //    mt_uIThingy->startInteraction();
  //
  //
  //    delete mt_uIThingy;

//  targetPointLocator->Delete();
//  sourcePolyData->Delete();
//  targetPolyData->Delete();

  return this->getLinearTransform(myConcatenation);
}


ssc::ImagePtr SeansVesselReg::loadMinc(char* p_dataFile)
{
  time_t sec1 = clock();
  std::cout << "Reading " << p_dataFile << " -> ";
  std::cout.flush();

  //Read data input file
  vtkMINCImageReader *l_dataReader = vtkMINCImageReader::New();
  l_dataReader->SetFileName(p_dataFile);
  l_dataReader->Update();

  double l_dataOrigin[3];
  l_dataReader->GetOutput()->GetOrigin(l_dataOrigin);
  std::cout << (clock() - sec1) / (double) CLOCKS_PER_SEC << " secs...DONE -> Processing...";
  std::cout.flush();
  int l_dimensions[3];
  l_dataReader->GetOutput()->GetDimensions(l_dimensions);

  //set the transform
  vtkTransform *l_dataTransform = vtkTransform::New();
  l_dataTransform->SetMatrix(l_dataReader->GetDirectionCosines());
  l_dataTransform->Translate(l_dataReader->GetDataOrigin());
  l_dataTransform->GetInverse()->TransformPoint(l_dataOrigin, l_dataOrigin);
  l_dataTransform->Translate(l_dataOrigin);
  l_dataTransform->Scale(l_dataReader->GetOutput()->GetSpacing());

//  int l_startPosX, l_startPosY, l_startPosZ; //Beginings of neighborhood offsets
//  int l_stopPosX, l_stopPosY, l_stopPosZ; //Ends of neighborhood offsets
//  int i, j, k, ii, jj, kk, l_counts = 0; //Counter variables
//
//  bool l_isNeighborFound; //Boolean for loop breakout
//
//  //dimensions, fast if we first deref it in to variables
//  int l_dimX = l_dimensions[0];
//  int l_dimY = l_dimensions[1];
//  int l_dimZ = l_dimensions[2];
//
//  double* l_tempPoint;
//  //Offsets variables
//  int l_offsetI = 0;
//  int l_offsetJ = 0;
//  int l_offsetK = 0;
//  int l_kkjjOffset = 0;
//  int l_kkjjiiOffset = 0;
//  int l_offsetConstIJ = l_dimX * l_dimY;
//
//  vtkDataArray* l_allTheData = l_dataReader->GetOutput()->GetPointData()->GetScalars();

//  Image(const QString& uid, const vtkImageDataPtr& data, const QString& name="");

  ssc::Transform3D rMd(l_dataTransform->GetMatrix());
  // TODO: ensure rMd is correct in CustusX terms
  QString uid(QString(p_dataFile)+"_minc_"+QDateTime::currentDateTime().toString());
  ssc::ImagePtr image(new ssc::Image(uid, l_dataReader->GetOutput(), uid));
  image->get_rMd_History()->addRegistration(ssc::RegistrationTransform(rMd, QDateTime::currentDateTime(), "from Minc file"));
  return image;
}

/** Input an image representation of centerlines.
 *  Tranform to polydata, reject all data outside bounding box,
 *  unknown parameter: p_neighborhoodFilterThreshold
 *
 */
vtkPolyDataPtr SeansVesselReg::extractPolyData(ssc::ImagePtr image, int p_neighborhoodFilterThreshold, double p_BoundingBox[6])
{
  vtkPolyDataPtr p_thePolyData = vtkPolyDataPtr::New();

  int l_dimensions[3];
  image->getBaseVtkImageData()->GetDimensions(l_dimensions);

  //set the transform
  vtkTransformPtr l_dataTransform = vtkTransformPtr::New();
  l_dataTransform->SetMatrix(image->get_rMd().matrix());

  int l_startPosX, l_startPosY, l_startPosZ; //Beginings of neighborhood offsets
  int l_stopPosX, l_stopPosY, l_stopPosZ; //Ends of neighborhood offsets
  int i, j, k, ii, jj, kk, l_counts = 0; //Counter variables

  bool l_isNeighborFound; //Boolean for loop breakout

  //dimensions, fast if we first deref it in to variables
  int l_dimX = l_dimensions[0];
  int l_dimY = l_dimensions[1];
  int l_dimZ = l_dimensions[2];

  double* l_tempPoint;
  //Offsets variables
  int l_offsetI = 0;
  int l_offsetJ = 0;
  int l_offsetK = 0;
  int l_kkjjOffset = 0;
  int l_kkjjiiOffset = 0;
  int l_offsetConstIJ = l_dimX * l_dimY;

  vtkDataArray* l_allTheData = image->getBaseVtkImageData()->GetPointData()->GetScalars();
  vtkPoints *l_dataPoints = vtkPoints::New();
  vtkCellArray *l_dataCellArray = vtkCellArray::New();

  //Loop through the entire volume and precalculate the offsets for each
  //point along with the points neighborhood offset
  for (k = 0; k < l_dimZ; ++k)
  {
    //the start and stop offsets for the Z neighborhood (a gap in a cube)
    l_startPosZ = k - p_neighborhoodFilterThreshold;
    if (l_startPosZ < 0)
      l_startPosZ = 0;
    else
      l_startPosZ *= l_offsetConstIJ;

    l_stopPosZ = k + p_neighborhoodFilterThreshold;
    if (l_stopPosZ >= l_dimZ)
      l_stopPosZ = (l_dimZ - 1) * l_offsetConstIJ;
    else
      l_stopPosZ *= l_offsetConstIJ;

    l_offsetK = k * l_offsetConstIJ;

    for (j = 0; j < l_dimY; ++j)
    {
      //the start and stop offsets for the Y neighborhood (a hole through a cube)
      l_startPosY = j - p_neighborhoodFilterThreshold;
      if (l_startPosY < 0)
        l_startPosY = 0;
      else
        l_startPosY *= l_dimX;

      l_stopPosY = j + p_neighborhoodFilterThreshold;
      if (l_stopPosY >= l_dimY)
        l_stopPosY = (l_dimY - 1) * l_dimX;
      else
        l_stopPosY *= l_dimX;

      l_offsetJ = l_offsetK + (j * l_dimX);

      for (i = 0; i < l_dimX; ++i)
      {
        //the start and stop offsets for the X neighborhood (a voxel)
        l_startPosX = i - p_neighborhoodFilterThreshold;
        if (l_startPosX < 0)
          l_startPosX = 0;

        l_stopPosX = i + p_neighborhoodFilterThreshold;
        if (l_stopPosX >= l_dimX)
          l_stopPosX = l_dimX - 1;

        l_offsetI = l_offsetJ + i;
        l_isNeighborFound = 0;

        //See if this voxel contain a vessel center, if so do some more processing
        if (*(l_allTheData->GetTuple(l_offsetI)))
        {
          l_tempPoint = l_dataTransform->TransformPoint(i, j, k);

          //Do stuff if there is no bounding box, or if there is one check if the
          //point is in the bounding box
          if (!p_BoundingBox || (p_BoundingBox[0] < l_tempPoint[0] && p_BoundingBox[1] > l_tempPoint[0]
              && p_BoundingBox[2] < l_tempPoint[1] && p_BoundingBox[3] > l_tempPoint[1] && p_BoundingBox[4]
              < l_tempPoint[2] && p_BoundingBox[5] > l_tempPoint[2]))
          {
            //Loop through the neigbors of the point and see if they are vessel centers
            //if one of them is it then write the point down
            for (kk = l_startPosZ; kk <= l_stopPosZ && !l_isNeighborFound; kk += l_offsetConstIJ)
            {
              for (jj = l_startPosY; jj <= l_stopPosY && !l_isNeighborFound; jj += l_dimX)
              {
                l_kkjjOffset = kk + jj;

                for (ii = l_startPosX; ii <= l_stopPosX && !l_isNeighborFound; ++ii)
                {
                  l_kkjjiiOffset = ii + l_kkjjOffset;

                  if (l_offsetI != l_kkjjiiOffset && //ignore if it is the current point
                      *(l_allTheData->GetTuple(l_kkjjiiOffset))) //check if vessel center
                  {
                    l_isNeighborFound = 1;
                    l_dataPoints->InsertNextPoint(l_tempPoint);
                    l_dataCellArray->InsertNextCell(1);
                    l_dataCellArray->InsertCellPoint(l_counts++);
                  }
                }
              }
            }
          }
        }
      }
    }
  }

  p_thePolyData->SetPoints(l_dataPoints);
  p_thePolyData->SetVerts(l_dataCellArray);

//  l_dataTransform->Delete();
//  l_dataReader->Delete();

  std::cout << "DONE" << std::endl;

  return p_thePolyData;
  //return vtkPolyDataPtr();
}

/**Convert the linear transform part of myContatenation to a ssc::Transform3D
 */
ssc::Transform3D SeansVesselReg::getLinearTransform(vtkGeneralTransform* myConcatenation)
{

  vtkMatrix4x4* l_tempMatrix = vtkMatrix4x4::New();
  vtkMatrix4x4* l_resultMatrix = vtkMatrix4x4::New();

  if (mt_doOnlyLinear)
    l_tempMatrix->DeepCopy(((vtkLandmarkTransform*) myConcatenation->GetConcatenatedTransform(0))->GetMatrix());

  l_resultMatrix->Identity();
  for (int i = 1; i < myConcatenation->GetNumberOfConcatenatedTransforms(); ++i)
  {
    vtkMatrix4x4::Multiply4x4(l_tempMatrix,
        ((vtkLandmarkTransform*) myConcatenation->GetConcatenatedTransform(i))->GetMatrix(), l_resultMatrix);
    l_tempMatrix->DeepCopy(l_resultMatrix);

  }

  return ssc::Transform3D(l_resultMatrix);
}
