

#include "cxAirwaysFromCenterline.h"
#include <vtkPolyData.h>
#include "cxBranchList.h"
#include "cxBranch.h"
#include "vtkCardinalSpline.h"
#include <vtkLine.h>
#include <vtkTubeFilter.h>
#include <vtkLineSource.h>
#include <vtkContourFilter.h>
#include <vtkPolyDataToImageStencil.h>
#include <cxImage.h>
#include "cxContourFilter.h"
#include <vtkImageData.h>
#include <vtkSphereSource.h>
#include <vtkDataSetSurfaceFilter.h>
#include <vtkPointData.h>
#include <vtkImageStencil.h>


namespace cx
{

AirwaysFromCenterline::AirwaysFromCenterline():
    mBranchListPtr(new BranchList)
{
}


AirwaysFromCenterline::~AirwaysFromCenterline()
{
}


Eigen::MatrixXd AirwaysFromCenterline::getCenterlinePositions(vtkPolyDataPtr centerline_r)
{

    int N = centerline_r->GetNumberOfPoints();
    Eigen::MatrixXd CLpoints(3,N);
    for(vtkIdType i = 0; i < N; i++)
        {
        double p[3];
        centerline_r->GetPoint(i,p);
        Eigen::Vector3d position;
		position(0) = p[0]; position(1) = p[1]; position(2) = p[2];
		CLpoints.block(0 , i , 3 , 1) = position;
		}
	return CLpoints;
}

void AirwaysFromCenterline::processCenterline(vtkPolyDataPtr centerline_r)
{
	if (mBranchListPtr)
		mBranchListPtr->deleteAllBranches();

    Eigen::MatrixXd CLpoints_r = getCenterlinePositions(centerline_r);

    mBranchListPtr->findBranchesInCenterline(CLpoints_r);

	mBranchListPtr->calculateOrientations();
	mBranchListPtr->smoothOrientations();
    mBranchListPtr->smoothBranchPositions(40);

	std::cout << "Number of branches in CT centerline: " << mBranchListPtr->getBranches().size() << std::endl;
}

vtkPolyDataPtr AirwaysFromCenterline::generateTubes()
{
    std::vector<BranchPtr> branches = mBranchListPtr->getBranches();

    //------- create image data ---------------
    vtkImageDataPtr resultImage = vtkImageDataPtr::New();
    vtkPointsPtr pointsPtr = vtkPointsPtr::New();

    int numberOfPoints = 0;
    for (int i = 0; i < branches.size(); i++)
        numberOfPoints += branches[i]->getPositions().cols();

    pointsPtr->SetNumberOfPoints(numberOfPoints);

    int pointIndex = 0;
    for (int i = 0; i < branches.size(); i++)
    {
        Eigen::MatrixXd positions = branches[i]->getPositions();
        for (int j = 0; j < positions.cols(); j++)
        {
            pointsPtr->SetPoint(pointIndex, positions(0,j), positions(1,j), positions(2,j));
            pointIndex += 1;
        }
    }

    double bounds[6];
    pointsPtr->GetBounds(bounds);

    //Extend bounds to make room for surface model extended from centerline
    bounds[0] -= 10;
    bounds[1] += 10;
    bounds[2] -= 10;
    bounds[3] += 10;
    bounds[4] -= 10;
    bounds[5] -= 2; // to make top of trachea open

    double spacing[3];
    spacing[0] = 0.5;  //Smaller spacing improves resolution but increases run-time
    spacing[1] = 0.5;
    spacing[2] = 0.5;
    resultImage->SetSpacing(spacing);

    // compute dimensions
    int dim[3];
    for (int i = 0; i < 3; i++)
        dim[i] = static_cast<int>(std::ceil((bounds[i * 2 + 1] - bounds[i * 2]) / spacing[i]));


    resultImage->SetDimensions(dim);
    resultImage->SetExtent(0, dim[0] - 1, 0, dim[1] - 1, 0, dim[2] - 1);

    double origin[3];
    origin[0] = bounds[0] + spacing[0] / 2;
    origin[1] = bounds[2] + spacing[1] / 2;
    origin[2] = bounds[4] + spacing[2] / 2;
    resultImage->SetOrigin(origin);

    resultImage->AllocateScalars(VTK_UNSIGNED_CHAR,1);


    //-----Generate tumes and spheres for each branch----------
    vtkLineSourcePtr lineSourcePtr = vtkLineSourcePtr::New();
    for (int i = 0; i < branches.size(); i++)
    {
        Eigen::MatrixXd positions = branches[i]->getPositions();
        vtkPointsPtr pointsPtr = vtkPointsPtr::New();
        int numberOfBranchPositions = positions.cols();
        pointsPtr->SetNumberOfPoints(numberOfBranchPositions);
        int displaceIndex = 0;

        if (branches[i]->getParentBranch()) // Add parents last position to get connected tubes
        {
            displaceIndex = 1;
            pointsPtr->SetNumberOfPoints(numberOfBranchPositions + 1);
            Eigen::MatrixXd parentPositions = branches[i]->getParentBranch()->getPositions();
            int numberOfParentBranchPositions = parentPositions.cols();
            pointsPtr->SetPoint(0, parentPositions(0,numberOfParentBranchPositions-1),
                                parentPositions(1,numberOfParentBranchPositions-1),
                                parentPositions(2,numberOfParentBranchPositions-1));
        }

        for (int j = 0; j < numberOfBranchPositions; j++)
            pointsPtr->SetPoint(j + displaceIndex, positions(0,j), positions(1,j), positions(2,j));

        lineSourcePtr->SetPoints(pointsPtr);

        // Create a tube (cylinder) around the line
         vtkTubeFilterPtr tubeFilterPtr = vtkTubeFilterPtr::New();
         tubeFilterPtr->SetInputConnection(lineSourcePtr->GetOutputPort());
         tubeFilterPtr->SetRadius(branches[i]->findBranchRadius()); //default is .5
         tubeFilterPtr->SetNumberOfSides(50);
         tubeFilterPtr->Update();

         //Add mesh from tube to image
         vtkPolyDataToImageStencilPtr pol2stencTube = vtkPolyDataToImageStencilPtr::New();
         pol2stencTube->SetInputData(tubeFilterPtr->GetOutput());
         pol2stencTube->SetOutputOrigin(origin);
         pol2stencTube->SetOutputSpacing(spacing);
         pol2stencTube->SetOutputWholeExtent(resultImage->GetExtent());
         pol2stencTube->Update();

         vtkImageStencilPtr imgstencTube = vtkImageStencilPtr::New();
         imgstencTube->SetInputData(resultImage);
         imgstencTube->SetStencilData(pol2stencTube->GetOutput());
         imgstencTube->ReverseStencilOn();
         imgstencTube->Update();

         resultImage = imgstencTube->GetOutput();


         //Add sphere at end of branch
         vtkSphereSourcePtr spherePtr = vtkSphereSourcePtr::New();
         spherePtr->SetCenter(positions(0,numberOfBranchPositions-1),
                           positions(1,numberOfBranchPositions-1),
                           positions(2,numberOfBranchPositions-1));
         spherePtr->SetRadius(1.05 * branches[i]->findBranchRadius()); // 5% larger radius to close holes
         spherePtr->SetThetaResolution(50);
         spherePtr->SetPhiResolution(50);
         spherePtr->Update();

         //Add sphere to image
         vtkPolyDataToImageStencilPtr pol2stencSphere = vtkPolyDataToImageStencilPtr::New();
         pol2stencSphere->SetInputData(spherePtr->GetOutput());
         pol2stencSphere->SetOutputOrigin(origin);
         pol2stencSphere->SetOutputSpacing(spacing);
         pol2stencSphere->SetOutputWholeExtent(resultImage->GetExtent());
         pol2stencSphere->Update();


         vtkImageStencilPtr imgstencSphere = vtkImageStencilPtr::New();
         imgstencSphere->SetInputData(resultImage);
         imgstencSphere->SetStencilData(pol2stencSphere->GetOutput());
         imgstencSphere->ReverseStencilOn();
         imgstencSphere->Update();

         resultImage = imgstencSphere->GetOutput();
    }


    //create contour from image
    vtkPolyDataPtr rawContour = ContourFilter::execute(
                resultImage,
            1, //treshold
            false, // reduce resolution
            true, // smoothing
            true, // keep topology
            0 // target decimation
    );

    return rawContour;

}

double findDistanceToLine(Eigen::MatrixXd point, Eigen::MatrixXd line)
{
    double minDistance = findDistance(point, line.col(0));
    for (int i=1; i<line.cols(); i++)
        if (minDistance > findDistance(point, line.col(i)))
            minDistance = findDistance(point, line.col(i));

    return minDistance;
}

double findDistance(Eigen::MatrixXd p1, Eigen::MatrixXd p2)
{
	double d0 = p1(0) - p2(0);
	double d1 = p1(1) - p2(1);
	double d2 = p1(2) - p2(2);

	double D = sqrt( d0*d0 + d1*d1 + d2*d2 );

	return D;
}


} /* namespace cx */
