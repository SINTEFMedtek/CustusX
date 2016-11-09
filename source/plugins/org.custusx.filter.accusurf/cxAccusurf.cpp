

#include "cxAccusurf.h"
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkImageData.h>
//#include "cxDoubleDataAdapterXml.h"
#include <vtkCellArray.h>
#include "vtkCardinalSpline.h"
#include "cxVolumeHelpers.h"
#include <boost/math/special_functions/round.hpp>
#include "cxReporter.h"
#include "cxBoundingBox3D.h"
#include "cxImageAlgorithms.h"
#include "cxImage.h"

typedef vtkSmartPointer<class vtkCardinalSpline> vtkCardinalSplinePtr;

namespace cx
{

Accusurf::Accusurf()
{
}


Accusurf::~Accusurf()
{
}

void Accusurf::setRoutePositions(vtkPolyDataPtr route)
{
    mRoutePositions.clear();
    int N = route->GetNumberOfPoints();
    for(vtkIdType i = 0; i < N; i++)
        {
        double p[3];
        route->GetPoint(i,p);
        Eigen::Vector3d position;
        position(0) = p[0]; position(1) = p[1]; position(2) = p[2];
        mRoutePositions.push_back(position);
        }
    smoothPositions();
}

void Accusurf::setInputImage(ImagePtr inputImage)
{ 
    mInputImage = inputImage->getBaseVtkImageData();
    mVtkScalarType = mInputImage->GetScalarType();
    mMinVoxelValue = inputImage->getMin();
}

void Accusurf::setThickness(int thicknessUp, int thicknessDown)
{
    mThicknessUp = thicknessUp;
    mThicknessDown = thicknessDown;
}

vtkImageDataPtr Accusurf::createNewEmptyImage()
{
    vtkImageDataPtr newImage = vtkImageDataPtr::New();
    newImage->DeepCopy(mInputImage);

    switch (mVtkScalarType)
    {
        case VTK_CHAR:
            newImage->AllocateScalars(VTK_CHAR, 1);
            this->insertValuesAtInitialization(static_cast<char*> (newImage->GetScalarPointer()), newImage);
            break;
        case VTK_UNSIGNED_CHAR:
            newImage->AllocateScalars(VTK_UNSIGNED_CHAR, 1);
            this->insertValuesAtInitialization(static_cast<unsigned char*> (newImage->GetScalarPointer()), newImage);
            break;
        case VTK_SIGNED_CHAR:
            newImage->AllocateScalars(VTK_SIGNED_CHAR, 1);
            this->insertValuesAtInitialization(static_cast<signed char*> (newImage->GetScalarPointer()), newImage);
            break;
        case VTK_UNSIGNED_SHORT:
            newImage->AllocateScalars(VTK_UNSIGNED_SHORT, 1);
            this->insertValuesAtInitialization(static_cast<unsigned short*> (newImage->GetScalarPointer()), newImage);
            break;
        case VTK_SHORT:
            newImage->AllocateScalars(VTK_SHORT, 1);
            this->insertValuesAtInitialization(static_cast<short*> (newImage->GetScalarPointer()), newImage);
            break;
        case VTK_UNSIGNED_INT:
            newImage->AllocateScalars(VTK_UNSIGNED_INT, 1);
            this->insertValuesAtInitialization(static_cast<unsigned int*> (newImage->GetScalarPointer()), newImage);
            break;
        case VTK_INT:
            newImage->AllocateScalars(VTK_INT, 1);
            this->insertValuesAtInitialization(static_cast<int*> (newImage->GetScalarPointer()), newImage);
            break;
        default:
            reportError(QString("Unknown VTK ScalarType: %1").arg(mVtkScalarType));
            return newImage;
            break;
    }

    setDeepModified(newImage);
    return newImage;
}

vtkImageDataPtr Accusurf::createAccusurfImage()
{
    vtkImageDataPtr accusurfImage = createNewEmptyImage();
    int* dim = accusurfImage->GetDimensions();
    double* spacing = accusurfImage->GetSpacing();
    std::vector<int> yIndexes (dim[2],0);

    for (int i = 0; i<mRoutePositions.size(); i++)
    {
        int y = (int) boost::math::round(mRoutePositions[i](1)/spacing[1]);
        int z = (int) boost::math::round(mRoutePositions[i](2)/spacing[2]);
        //std::cout << "y: " << y << " - z: " << z << " - size YIndexes: " << yIndexes.size() << std::endl;
        if (z >= 0 && z < dim[2] && y >= 0 && y < dim[1])
            yIndexes[z] = y;
    }

    for (int i = 1; i<dim[2]; i++)
    {
        if (yIndexes[i] == 0)
            yIndexes[i] = yIndexes[i-1];
    }
    for (int i = dim[2]-2; i>=0; i--)
    {
        if (yIndexes[i] == 0)
            yIndexes[i] = yIndexes[i+1];
    }


    switch (mVtkScalarType)
    {
        case VTK_CHAR:
            this->insertValuesFromOriginalImage<char>(accusurfImage, dim, yIndexes);
            break;
        case VTK_UNSIGNED_CHAR:
            this->insertValuesFromOriginalImage<unsigned char>(accusurfImage, dim, yIndexes);
            break;
        case VTK_SIGNED_CHAR:
            this->insertValuesFromOriginalImage<signed char>(accusurfImage, dim, yIndexes);
            break;
        case VTK_UNSIGNED_SHORT:
            insertValuesFromOriginalImage<unsigned short>(accusurfImage, dim, yIndexes);
            break;
        case VTK_SHORT:
            this->insertValuesFromOriginalImage<short>(accusurfImage, dim, yIndexes);
            break;
        case VTK_UNSIGNED_INT:
            this->insertValuesFromOriginalImage<unsigned int>(accusurfImage, dim, yIndexes);
            break;
        case VTK_INT:
            this->insertValuesFromOriginalImage<int>(accusurfImage, dim, yIndexes);
            break;
        default:
            reportError(QString("Unknown VTK ScalarType: %1").arg(mVtkScalarType));
            break;
    }

    int yMin = *std::min_element(yIndexes.begin(), yIndexes.end());
    int yMax = *std::max_element(yIndexes.begin(), yIndexes.end());
    accusurfImage = crop(accusurfImage, yMin, yMax);
    setDeepModified(accusurfImage);
    return accusurfImage;
}

void Accusurf::smoothPositions()
{
    int numberOfInputPoints = mRoutePositions.size();
    int controlPointFactor = 10;
    int numberOfControlPoints = numberOfInputPoints / controlPointFactor;

    vtkCardinalSplinePtr splineX = vtkSmartPointer<vtkCardinalSpline>::New();
    vtkCardinalSplinePtr splineY = vtkSmartPointer<vtkCardinalSpline>::New();
    vtkCardinalSplinePtr splineZ = vtkSmartPointer<vtkCardinalSpline>::New();

    if (numberOfControlPoints >= 2)
    {
        //add control points to spline
        for(int j=0; j<numberOfControlPoints; j++)
        {
            int indexP = (j*numberOfInputPoints)/numberOfControlPoints;
            splineX->AddPoint(indexP,mRoutePositions[indexP](0));
            splineY->AddPoint(indexP,mRoutePositions[indexP](1));
            splineZ->AddPoint(indexP,mRoutePositions[indexP](2));
        }
        //Always add the last point to complete spline
        splineX->AddPoint(numberOfInputPoints-1,mRoutePositions[numberOfInputPoints-1](0));
        splineY->AddPoint(numberOfInputPoints-1,mRoutePositions[numberOfInputPoints-1](1));
        splineZ->AddPoint(numberOfInputPoints-1,mRoutePositions[numberOfInputPoints-1](2));

        //evaluate spline - get smoothed positions
        std::vector< Eigen::Vector3d > smoothingResult;
        for(int j=0; j<numberOfInputPoints; j++)
        {
            double splineParameter = j;
            Eigen::Vector3d tempPoint;
            tempPoint(0) = splineX->Evaluate(splineParameter);
            tempPoint(1) = splineY->Evaluate(splineParameter);
            tempPoint(2) = splineZ->Evaluate(splineParameter);
            smoothingResult.push_back(tempPoint);
        }
        mRoutePositions.clear();
        mRoutePositions = smoothingResult;
    }
}

vtkImageDataPtr Accusurf::crop(vtkImageDataPtr image, int ymin, int ymax){

    int* dim = image->GetDimensions();
    IntBoundingBox3D cropbox(0, dim[0], std::max(0, ymin-mThicknessUp-2) , std::min(dim[1]-1, ymax+mThicknessDown+2) , 0, dim[2]);
    vtkImageDataPtr cropedImage = cropImage(image, cropbox);
    return cropedImage;
}

template <class TYPE>
void Accusurf::insertValuesAtInitialization(TYPE* dataPtr, vtkImageDataPtr image)
{
    int* dim = image->GetDimensions();
    int numVoxels = dim[0]*dim[1]*dim[2];
    for (int i = 0; i < numVoxels; ++i)
    {
            dataPtr[i] = mMinVoxelValue;
    }
}

template <class TYPE>
void Accusurf::insertValuesFromOriginalImage(vtkImageDataPtr image, int* dim, std::vector<int> yIndexes)
{
    for (int z = 0; z<dim[2]; z++)
    {
        for (int x = 0; x<dim[0]; x++)
        {
            int ymin = std::max(yIndexes[z]-mThicknessUp,0);
            int ymax = std::min(yIndexes[z]+mThicknessDown,dim[1]-1);
            for (int y = ymin; y<=ymax; y++)
            {

                    TYPE* dataPtrInputImage = static_cast<TYPE*> (mInputImage->GetScalarPointer(x,y,z));
                    TYPE* dataPtrAccusurfImage = static_cast<TYPE*> (image->GetScalarPointer(x,y,z));
                    dataPtrAccusurfImage[0] = dataPtrInputImage[0];
             }
        }
    }
}

} /* namespace cx */
