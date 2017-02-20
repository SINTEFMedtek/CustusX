#ifndef CXACCUSURF_H
#define CXACCUSURF_H

#include "cxMesh.h"
#include <QDomElement>
#include "org_custusx_filter_accusurf_Export.h"


namespace cx
{

typedef boost::shared_ptr<class RouteToTarget> RouteToTargetPtr;

class org_custusx_filter_accusurf_EXPORT Accusurf
{
public:
    Accusurf();
    virtual ~Accusurf();
    void setRoutePositions(vtkPolyDataPtr route);
    void setInputImage(ImagePtr inputImage);
    void setThickness(int thicknessUp, int thicknessDown);
    vtkImageDataPtr createAccusurfImage();

private:

    vtkImageDataPtr createNewEmptyImage();
    vtkImageDataPtr crop(vtkImageDataPtr image, int ymin, int ymax);
    template <class TYPE>
    void insertValuesAtInitialization(TYPE* volumePointer, vtkImageDataPtr image);
    template <class TYPE>
    void insertValuesFromOriginalImage(vtkImageDataPtr image, int* dim, std::vector<int> yIndexes);

    void smoothPositions();
    std::vector< Eigen::Vector3d > mRoutePositions;
    vtkImageDataPtr mInputImage;
    int mVtkScalarType;
    int mMinVoxelValue;
    int mThicknessUp;
    int mThicknessDown;
};

} /* namespace cx */

#endif // CXACCUSURF_H
