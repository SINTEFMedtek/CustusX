#ifndef SSCSLICEDIMAGEPROXY_H_
#define SSCSLICEDIMAGEPROXY_H_

#include "sscRepImpl.h"
#include "sscTransform3D.h"

#include <vtkSmartPointer.h>
typedef vtkSmartPointer<class vtkImageData> vtkImageDataPtr;
typedef vtkSmartPointer<class vtkImageMapToWindowLevelColors> vtkImageMapToWindowLevelColorsPtr;
typedef vtkSmartPointer<class vtkImageMapToColors> vtkImageMapToColorsPtr;
typedef vtkSmartPointer<class vtkImageAlgorithm> vtkImageAlgorithmPtr;
typedef vtkSmartPointer<class vtkImageReslice> vtkImageReslicePtr;

namespace ssc
{
// forward declarations
typedef boost::shared_ptr<class Image> ImagePtr;
typedef boost::shared_ptr<class SliceProxy> SliceProxyPtr;

typedef boost::shared_ptr<class SlicedImageProxy> SlicedImageProxyPtr;

/**Helper class for slicing an image given a sliceproxy and an image.
 * Used internally by BlendedSliceRep and SlicerRepSW as the slice engine.
 * 
 * Used by Sonowand 2.1
 */
class SlicedImageProxy : public QObject
{
Q_OBJECT
public:
	SlicedImageProxy();
	virtual ~SlicedImageProxy();
	void setSliceProxy(ssc::SliceProxyPtr slicer);
	void setImage(ImagePtr image);
	ImagePtr getImage() const;
	void update();
	vtkImageDataPtr getOutput(); ///< output 2D sliced image	
	void printSelf(std::ostream & os, Indent indent);

private slots:
	void sliceTransformChangedSlot(Transform3D sMr);
	void transferFunctionsChangedSlot();

private: 	
	vtkImageDataPtr createDummyImageData();

	SliceProxyPtr mSlicer;
	vtkImageDataPtr mDummyImage; ///< need this to fool the vtk pipeline when no image is set
	ImagePtr mImage;
	//vtkImageMapToWindowLevelColorsPtr mWindowLevel;
	vtkImageAlgorithmPtr mRedirecter; 	
 	//vtkImageReslicePtr mRedirecter;
	
	vtkImageMapToColorsPtr mWindowLevel;
	vtkImageReslicePtr mReslicer;
	vtkMatrix4x4Ptr mMatrixAxes;
};

//---------------------------------------------------------
}//end namespace
//---------------------------------------------------------
#endif /*SSCSLICEDIMAGEPROXY_H_*/
