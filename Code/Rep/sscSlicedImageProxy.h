#ifndef SSCSLICEDIMAGEPROXY_H_
#define SSCSLICEDIMAGEPROXY_H_
#include <vtkSmartPointer.h>
#include "sscDefinitions.h"
#include "sscForwardDeclarations.h"
#include "sscTool.h"
#include "sscRepImpl.h"
#include "sscImage.h"
#include "sscSliceComputer.h"
#include "sscVector3D.h"
#include "sscTransform3D.h"
	
typedef vtkSmartPointer<class vtkImageMapToWindowLevelColors> vtkImageMapToWindowLevelColorsPtr;
typedef vtkSmartPointer<class vtkImageMapToColors> vtkImageMapToColorsPtr;

namespace ssc
{

typedef boost::shared_ptr<class SlicedImageProxy> SlicedImageProxyPtr;

/**Used internally by BlendedSliceRep, 
 * as the slice engine.
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
	ImagePtr getImage()const;
	void update();
	 vtkImageDataPtr getOutput();///<output 2D sliced image	
	 
	 
signals:
	void updateAlpha();
	
private slots:
	void sliceTransformChangedSlot(Transform3D sMr);

private:	
	SliceProxyPtr mSlicer;
	ImagePtr mImage;
	std::string mImageUid;

	vtkImageMapToWindowLevelColorsPtr mWindowLevel;
	vtkImageReslicePtr mReslicer;
	vtkMatrix4x4Ptr mMatrixAxes;
	
};


//---------------------------------------------------------
}//end namespace
//---------------------------------------------------------
#endif /*SSCSLICEDIMAGEPROXY_H_*/
