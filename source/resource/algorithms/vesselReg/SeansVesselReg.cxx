#include "SeansVesselReg.hxx"
#include "HackTPSTransform.hxx"

#include <iostream>
#include <time.h>
#include <fstream>

#include <QFileInfo>

#include "sscImage.h"
#include "sscTypeConversions.h"
#include "sscRegistrationTransform.h"
#include "sscMessageManager.h"
#include "sscDataManager.h"
#include <boost/math/special_functions/fpclassify.hpp>

//#include "cxStateMachineManager.h"
//#include "cxPatientData.h"
#include "vtkClipPolyData.h"
#include "vtkPlanes.h"

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
#include "sscMesh.h"

namespace cx
{

SeansVesselReg::SeansVesselReg(int lts_ratio, double stop_delta, double lambda, double sigma, bool lin_flag,
	int sample, int single_point_thre, bool verbose) :
	mt_ltsRatio(lts_ratio), mt_distanceDeltaStopThreshold(stop_delta), mt_lambda(lambda), mt_sigma(sigma),
		mt_doOnlyLinear(lin_flag), mt_sampleRatio(sample), mt_singlePointThreshold(single_point_thre),
		mt_maximumNumberOfIterations(100), mt_verbose(verbose), mInvertedTransform(false)
{
}

SeansVesselReg::~SeansVesselReg()
{
}

/**Execute the vessel to vessel registration.
 * The result is available via the getLinearTransform().
 *
 * source is moving and target is fixed
 */
bool SeansVesselReg::execute(ssc::DataPtr source, ssc::DataPtr target, QString logPath)
{
	if (mt_verbose)
	{
		ssc::messageManager()->sendDebug("SOURCE: " + source->getUid());
		ssc::messageManager()->sendDebug("TARGET: " + target->getUid());

		std::cout << "stop Threshold:" << mt_distanceDeltaStopThreshold << endl;
		std::cout << "sigma:" << mt_sigma << endl;
		std::cout << "lts Ratio:" << mt_ltsRatio << endl;
		std::cout << "linear flag:" << mt_doOnlyLinear << endl;
		std::cout << "sample flag:" << mt_sampleRatio << endl;
		std::cout << "single Point Threshold:" << mt_singlePointThreshold << endl;
	}
	time_t sec1 = clock();//time(NULL);

	vtkPolyDataPtr targetPolyData = this->convertToPolyData(target);
	vtkPolyDataPtr sourcePolyData = this->convertToPolyData(source);

	double margin = 40;
	sourcePolyData = this->crop(sourcePolyData, targetPolyData, margin);

	//Make sure we have stuff to work with
	if (!sourcePolyData->GetNumberOfPoints() || !targetPolyData->GetNumberOfPoints())
	{
		std::cerr << "Can't execute with empty source or target data" << std::endl;
		return false;
	}

	mInvertedTransform = false;

	// Algorithm requires #source < #target
	// swap if this is not the case
	if (sourcePolyData->GetNumberOfPoints() > targetPolyData->GetNumberOfPoints())
	{
		//INVERT
		if (mt_verbose)
			std::cout << "inverted vessel reg" << std::endl;
		mInvertedTransform = true;
		std::swap(sourcePolyData, targetPolyData);
	}

	vtkIdType numPoints = sourcePolyData->GetNumberOfPoints();
	if (mt_verbose)
	{
		std::cout << "total number of source points:" << numPoints << ", target points: " << targetPolyData->GetNumberOfPoints() << endl;
		std::cout << "number of source points to be sampled:" << ((int) (numPoints * mt_ltsRatio) / 100) << "\n" << endl;
	}

	//Container for all the transforms
	vtkGeneralTransformPtr myConcatenation = vtkGeneralTransformPtr::New();

	//Do EVERYTHING
	if (!this->runAlgorithm(sourcePolyData, targetPolyData, myConcatenation))
	{
		return false;
	}

	printOutResults(logPath + "/Vessel_Based_Registration_", myConcatenation);

	if (mt_verbose)
		std::cout << "\n\n\nExecution time:" << " " << (clock() - sec1) / (double) CLOCKS_PER_SEC << " " << "seconds"
			<< endl;

	mLinearTransformResult = this->getLinearTransform(myConcatenation);

	return true;
}

/**Run the core algorithm
 *
 */
bool SeansVesselReg::runAlgorithm(
	vtkPolyDataPtr currentSourcePolyData,
	vtkPolyDataPtr targetPolyData,
	vtkGeneralTransformPtr myConcatenation)
{
	// Create locator for target points
	vtkCellLocatorPtr targetPointLocator = vtkCellLocatorPtr::New();
	targetPointLocator->SetDataSet(targetPolyData);
	targetPointLocator->SetNumberOfCellsPerBucket(1);
	targetPointLocator->BuildLocator();

	//Since we are going to play with the data, we have to make a copy
	vtkPointsPtr currentSourcePoints = vtkPointsPtr::New();
	currentSourcePoints->DeepCopy(currentSourcePolyData->GetPoints());
	// total number of source points:
	int numPoints = currentSourcePoints->GetNumberOfPoints();
	// number of source points used in each iteration (the rest is temporarily rejected from the computation)
	int nb_points = ((int) (numPoints * mt_ltsRatio) / 100);

	// - closestPoint is used so that the internal state of LandmarkTransform remains
	//   correct whenever the iteration process is stopped (hence its source
	//   and landmark points might be used in a vtkThinPlateSplineTransform).
	vtkPointsPtr closestPoint = vtkPointsPtr::New();
	closestPoint->SetNumberOfPoints(numPoints);

	std::vector<double> mean_distance(mt_maximumNumberOfIterations);
	bool l_keepRunning = 1;
	double difference = 100;

	for (int myNumberOfIterations = 1; l_keepRunning && myNumberOfIterations < mt_maximumNumberOfIterations; ++myNumberOfIterations)
	{
		// Fill points with the closest points to each vertex in input
		vtkFloatArrayPtr residuals = vtkFloatArrayPtr::New();
		residuals->SetNumberOfValues(numPoints);

		vtkIdListPtr IdList = vtkIdListPtr::New();
		IdList->SetNumberOfIds(numPoints);
		double total_distance = 0;
		double distanceSquared = 0;
		//Find closest points to all source points
		for (int i = 0; i < numPoints; ++i)
		{
			//Check the distance to neighbouring points (neighbours should be matched to nearby points)
			vtkIdType cell_id;
			int sub_id;
			double outPoint[3];
			targetPointLocator->FindClosestPoint(currentSourcePoints->GetPoint(i), outPoint, cell_id, sub_id, distanceSquared);
			closestPoint->SetPoint(i, outPoint);
			if (boost::math::isnan(distanceSquared))
			{
				std::cout << "nan found during findClosestPoint!" << std::endl;
				return false;
			}
			residuals->InsertValue(i, distanceSquared);
			IdList->InsertId(i, i);
			total_distance += sqrt(distanceSquared);
		}
		mean_distance[myNumberOfIterations] = total_distance / numPoints;

		if (myNumberOfIterations != 1)
		{
			difference = mean_distance[myNumberOfIterations] - mean_distance[myNumberOfIterations - 1];
		}

		vtkSortDataArrayPtr sort = vtkSortDataArrayPtr::New();
		sort->Sort(residuals, IdList);
		vtkPointsPtr sortedSourcePoints = this->createSortedPoints(IdList, currentSourcePoints, nb_points);
		vtkPointsPtr sortedTargetPoints = this->createSortedPoints(IdList, closestPoint, nb_points);

		vtkAbstractTransformPtr myCurrentTransform;

		if (mt_doOnlyLinear)
		{
			myCurrentTransform = linearRegistration(sortedSourcePoints, sortedTargetPoints, numPoints);
		}
		else
		{
			myCurrentTransform = linearRegistration(sortedSourcePoints, sortedTargetPoints, numPoints);

			if (fabs(difference) < mt_distanceDeltaStopThreshold)
			{
				myCurrentTransform = nonLinearRegistration(sortedSourcePoints, sortedTargetPoints, numPoints);
			}
		}

		// Transform the source points with the transform found during this iteration,
		// in order to use an updated guess for the next iteration
		currentSourcePoints = this->transformPoints(currentSourcePoints, myCurrentTransform);

		// add transform from this iteration to the total
		myConcatenation->Concatenate(myCurrentTransform);

		// Check for convergence
		if (fabs(difference) < mt_distanceDeltaStopThreshold)
		{
			l_keepRunning = 0;
		}

		if (mt_verbose)
		{
//			std::cout << myNumberOfIterations << " ";
//			std::cout.flush();
			std::cout << QString("%1\t%2").arg(myNumberOfIterations).arg(mean_distance[myNumberOfIterations]) << std::endl;
		}
	}

	if (mt_verbose)
		std::cout << endl;

	myConcatenation->Update();
	return true;
}

/**Using the already sorted list of point ID's, create a sorted list of points
 * based on the numPoint first of unsortedPoints.
 *
 */
vtkPointsPtr SeansVesselReg::createSortedPoints(vtkIdListPtr sortedIDList, vtkPointsPtr unsortedPoints, int numPoints)
{
	vtkPointsPtr retval = vtkPointsPtr::New();
	retval->SetNumberOfPoints(numPoints);

	double temp_point[3];

	for (int i = 0; i < numPoints; ++i)
	{
		vtkIdType index = sortedIDList->GetId(i);
		unsortedPoints->GetPoint(index, temp_point); // source points to use in tps
		retval->SetPoint(i, temp_point);
	}

	return retval;
}


vtkPointsPtr SeansVesselReg::transformPoints(vtkPointsPtr input, vtkAbstractTransformPtr transform)
{
	int numPoints = input->GetNumberOfPoints();
	vtkPointsPtr retval = vtkPointsPtr::New();
	retval->SetNumberOfPoints(numPoints);

	//Transform ALL source points
	double tempPostTransPoint[3];
	for (int i = 0; i < numPoints; ++i)
	{
		transform->InternalTransformPoint(input->GetPoint(i), tempPostTransPoint);
		retval->SetPoint(i, tempPostTransPoint);
	}

	return retval;
}

vtkAbstractTransformPtr SeansVesselReg::linearRegistration(vtkPointsPtr sortedSourcePoints,
	vtkPointsPtr sortedTargetPoints, int numPoints/*, vtkAbstractTransform** myCurrentTransform*/)
{
	//Build landmark transform
	vtkLandmarkTransformPtr lmt = vtkLandmarkTransformPtr::New();
	lmt->SetSourceLandmarks(sortedSourcePoints);
	lmt->SetTargetLandmarks(sortedTargetPoints);
	lmt->SetModeToRigidBody();
	lmt->Modified();
	lmt->Update();

	//  *myCurrentTransform = lmt;
	return lmt;
}

vtkAbstractTransformPtr SeansVesselReg::nonLinearRegistration(vtkPointsPtr sortedSourcePoints,
	vtkPointsPtr sortedTargetPoints, int numPoints)
{
	vtkPolyDataPtr tpsSourcePolyData = this->convertToPolyData(sortedSourcePoints);
	vtkPolyDataPtr tpsTargetPolyData = this->convertToPolyData(sortedTargetPoints);

	vtkMaskPointsPtr mask1 = vtkMaskPointsPtr::New();
	mask1->SetInput(tpsSourcePolyData);
	mask1->SetOnRatio(mt_sampleRatio);
	mask1->Update();
	vtkMaskPointsPtr mask2 = vtkMaskPointsPtr::New();
	mask2->SetInput(tpsTargetPolyData);
	mask2->SetOnRatio(mt_sampleRatio);
	mask2->Update();

	// Build the thin plate spline transform
	vtkThinPlateSplineTransformPtr tps = vtkThinPlateSplineTransformPtr::New();
	tps->SetSourceLandmarks(mask1->GetOutput()->GetPoints());
	tps->SetTargetLandmarks(mask2->GetOutput()->GetPoints());
	tps->SetBasisToR();
	tps->SetSigma(mt_sigma);

	//*myCurrentTransform = tps;
	return tps;
}

vtkPolyDataPtr SeansVesselReg::convertToPolyData(ssc::DataPtr data)
{
	ssc::ImagePtr image = boost::dynamic_pointer_cast<ssc::Image>(data);
	ssc::MeshPtr mesh = boost::dynamic_pointer_cast<ssc::Mesh>(data);

	if (image)
	{
		//Grab the information from the files of target then
		//filter out points not fit for the threshold
		return this->extractPolyData(image, mt_singlePointThreshold, 0);
	}
	else if (mesh)
	{
		return mesh->getTransformedPolyData(mesh->get_rMd());
	}

	return vtkPolyDataPtr();
}

vtkPolyDataPtr SeansVesselReg::convertToPolyData(vtkPointsPtr input)
{
	vtkCellArrayPtr cellArray = vtkCellArrayPtr::New();
	int N = input->GetNumberOfPoints();

	for (int i=0; i<N ; ++i)
	{
		cellArray->InsertNextCell(1);
		cellArray->InsertCellPoint(i);
	}

	vtkPolyDataPtr retval = vtkPolyDataPtr::New();
	retval->SetPoints(input);
	retval->SetVerts(cellArray);

	return retval;
}

void SeansVesselReg::print(vtkPointsPtr points)
{
	for (int q = 0; q < points->GetNumberOfPoints(); ++q)
	{
		ssc::Vector3D p(points->GetPoint(q));
		std::cout << q << "\t" << p[0] << " " << p[1] << " " << p[2] << " " << std::endl;
	}
}

void SeansVesselReg::print(vtkPolyDataPtr data)
{
	print(data->GetPoints());
}

/**Crop the input data using a bounding box generated from the fixed data.
 * The margin is used to enlarge the bounding box.
 *
 */
vtkPolyDataPtr SeansVesselReg::crop(vtkPolyDataPtr input, vtkPolyDataPtr fixed, double margin)
{
	// use the bounding box of target to clip the source data.
	fixed->GetPoints()->ComputeBounds();
	double* targetBounds = fixed->GetPoints()->GetBounds();
	targetBounds[0] -= margin;
	targetBounds[1] += margin;
	targetBounds[2] -= margin;
	targetBounds[3] += margin;
	targetBounds[4] -= margin;
	targetBounds[5] += margin;

	// clip the source data with a box
	vtkPlanesPtr box = vtkPlanesPtr::New();
	//  std::cout << "bounds" << std::endl;
	box->SetBounds(targetBounds);
	if (mt_verbose)
		std::cout << "bb: " << ssc::DoubleBoundingBox3D(targetBounds) << std::endl;
	vtkClipPolyDataPtr clipper = vtkClipPolyDataPtr::New();
	clipper->SetInput(input);
	clipper->SetClipFunction(box);
	clipper->SetInsideOut(true);
	clipper->Update();

	int oldSource = input->GetPoints()->GetNumberOfPoints();
	int clippedSource = clipper->GetOutput()->GetPoints()->GetNumberOfPoints();

	if (clippedSource < oldSource)
	{
		double ratio = double(oldSource - clippedSource) / double(oldSource);
		if (mt_verbose)
			std::cout << "Removed " << ratio * 100 << "%" << " of the source data. Outside the target data bounds." << std::endl;
	}

	return clipper->GetOutput();
}

ssc::Transform3D SeansVesselReg::getLinearTransform()
{
	ssc::Transform3D retval = mLinearTransformResult;
	if (mInvertedTransform)
		retval = retval.inv();

	return retval;
}

/**Convert the linear transform part of myContatenation to a ssc::Transform3D
 */
ssc::Transform3D SeansVesselReg::getLinearTransform(vtkGeneralTransformPtr myConcatenation)
{
	vtkMatrix4x4Ptr l_tempMatrix = vtkMatrix4x4Ptr::New();
	vtkMatrix4x4Ptr l_resultMatrix = vtkMatrix4x4Ptr::New();

	if (mt_doOnlyLinear)
		l_tempMatrix->DeepCopy(((vtkLandmarkTransform*) myConcatenation->GetConcatenatedTransform(0))->GetMatrix());

	l_resultMatrix->Identity();
	for (int i = 1; i < myConcatenation->GetNumberOfConcatenatedTransforms(); ++i)
	{
		vtkMatrix4x4::Multiply4x4(l_tempMatrix,
			((vtkLandmarkTransform*) myConcatenation->GetConcatenatedTransform(i))->GetMatrix(), l_resultMatrix);
		l_tempMatrix->DeepCopy(l_resultMatrix);

	}

	return ssc::Transform3D(l_resultMatrix).inverse();
}

void SeansVesselReg::printOutResults(QString fileNamePrefix, vtkGeneralTransformPtr myConcatenation)
{

	vtkMatrix4x4Ptr l_tempMatrix = vtkMatrix4x4Ptr::New();
	vtkMatrix4x4Ptr l_resultMatrix = vtkMatrix4x4Ptr::New();

	if (mt_doOnlyLinear)
		l_tempMatrix->DeepCopy(((vtkLandmarkTransform*) myConcatenation->GetConcatenatedTransform(0))->GetMatrix());

	l_resultMatrix->Identity();
	for (int i = 1; i < myConcatenation->GetNumberOfConcatenatedTransforms(); ++i)
	{
		vtkMatrix4x4::Multiply4x4(l_tempMatrix,
			((vtkLandmarkTransform*) myConcatenation->GetConcatenatedTransform(i))->GetMatrix(), l_resultMatrix);
		l_tempMatrix->DeepCopy(l_resultMatrix);

	}
	if (mt_verbose)
		std::cout << "Filenameprefix: " << fileNamePrefix << std::endl;

	//std::string logsFolder = string_cast(cx::stateService()->getPatientData()->getActivePatientFolder())+"/Logs/";
	//std::string logsFolder = "~/Patients/Logs/";
	std::string nonLinearFile = fileNamePrefix.toStdString();
	nonLinearFile += "--NonLinear";
	nonLinearFile += ".txt";

	std::string linearFile = fileNamePrefix.toStdString();
	linearFile += "--Linear";
	linearFile += ".txt";

	if (mt_verbose)
		std::cout << "Writing Results to " << nonLinearFile << " and " << linearFile << std::endl;

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
	file_out2.close();
}
ssc::ImagePtr SeansVesselReg::loadMinc(char* p_dataFile)
{
	time_t sec1 = clock();
	std::cout << "Reading " << p_dataFile << " -> ";
	std::cout.flush();

	//Read data input file
	vtkMINCImageReaderPtr l_dataReader = vtkMINCImageReaderPtr::New();
	l_dataReader->SetFileName(p_dataFile);
	l_dataReader->Update();

	double l_dataOrigin[3];
	l_dataReader->GetOutput()->GetOrigin(l_dataOrigin);
	std::cout << (clock() - sec1) / (double) CLOCKS_PER_SEC << " secs...DONE -> Processing...";
	std::cout.flush();
	int l_dimensions[3];
	l_dataReader->GetOutput()->GetDimensions(l_dimensions);

	//set the transform
	vtkTransformPtr l_dataTransform = vtkTransformPtr::New();
	l_dataTransform->SetMatrix(l_dataReader->GetDirectionCosines());
	l_dataTransform->Translate(l_dataReader->GetDataOrigin());
	l_dataTransform->GetInverse()->TransformPoint(l_dataOrigin, l_dataOrigin);
	l_dataTransform->Translate(l_dataOrigin);
	l_dataTransform->Scale(l_dataReader->GetOutput()->GetSpacing());

	ssc::Transform3D rMd(l_dataTransform->GetMatrix());

	// TODO: ensure rMd is correct in CustusX terms

	QFile file(p_dataFile);
	QFileInfo info(file);
	QString uid(info.completeBaseName() + "_minc_%1");
	QString name = uid;

	ssc::ImagePtr image = ssc::dataManager()->createImage(l_dataReader->GetOutput(), uid, name);
	image->get_rMd_History()->addRegistration(ssc::RegistrationTransform(rMd, QDateTime::currentDateTime(),
		"from Minc file"));

	return image;
}

/** Input an image representation of centerlines.
 *  Tranform to polydata, reject all data outside bounding box,
 *  unknown parameter: p_neighborhoodFilterThreshold
 *
 */
vtkPolyDataPtr SeansVesselReg::extractPolyData(ssc::ImagePtr image, int p_neighborhoodFilterThreshold,
	double p_BoundingBox[6])
{
	vtkPolyDataPtr p_thePolyData = vtkPolyDataPtr::New();

	int l_dimensions[3];
	image->getBaseVtkImageData()->GetDimensions(l_dimensions);
	ssc::Vector3D spacing(image->getBaseVtkImageData()->GetSpacing());

	//set the transform
	vtkTransformPtr l_dataTransform = vtkTransformPtr::New();
	l_dataTransform->SetMatrix(image->get_rMd().getVtkMatrix());

	int l_startPosX, l_startPosY, l_startPosZ; //Beginings of neighborhood offsets
	int l_stopPosX, l_stopPosY, l_stopPosZ; //Ends of neighborhood offsets
	int i, j, k, ii, jj, kk, l_counts = 0; //Counter variables

	bool l_isNeighborFound; //Boolean for loop breakout

	//dimensions, fast if we first deref it in to variables
	int l_dimX = l_dimensions[0];
	int l_dimY = l_dimensions[1];
	int l_dimZ = l_dimensions[2];

	double l_tempPoint[3];
	//Offsets variables
	int l_offsetI = 0;
	int l_offsetJ = 0;
	int l_offsetK = 0;
	int l_kkjjOffset = 0;
	int l_kkjjiiOffset = 0;
	int l_offsetConstIJ = l_dimX * l_dimY;

	vtkDataArrayPtr l_allTheData = image->getBaseVtkImageData()->GetPointData()->GetScalars();
	vtkPointsPtr l_dataPoints = vtkPointsPtr::New();
	vtkCellArrayPtr l_dataCellArray = vtkCellArrayPtr::New();

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
					// added by CA: use spacing when creating point. TODO: check  with Ingrid if any other data are affected.
					l_tempPoint[0] = spacing[0] * i;
					l_tempPoint[1] = spacing[1] * j;
					l_tempPoint[2] = spacing[2] * k;
					l_dataTransform->TransformPoint(l_tempPoint, l_tempPoint);

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

	return p_thePolyData;
}

} //namespace cx
