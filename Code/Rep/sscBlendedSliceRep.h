#ifndef SSCLAYERSLICEREP_H_
#define SSCLAYERSLICEREP_H_
#include <vtkSmartPointer.h>
#include <vector>
#include "sscDefinitions.h"
#include "sscForwardDeclarations.h"
#include "sscTool.h"
#include "sscRepImpl.h"
#include "sscImage.h"
#include "sscSliceComputer.h"
#include "sscVector3D.h"
#include "sscTransform3D.h"
//#include "sscImageBlenderProxy.h"
#include "sscSlicerRepSW.h"
#include "sscSlicedImageProxy.h"

typedef vtkSmartPointer<class vtkImageResample> vtkImageResamplePtr;
typedef vtkSmartPointer<class vtkImageBlend> vtkImageBlendPtr;
typedef vtkSmartPointer<class vtkTexture> vtkTexturePtr;
typedef vtkSmartPointer<class vtkPolyDataMapper> vtkPolyDataMapperPtr;

namespace ssc
{
typedef boost::shared_ptr<class BlendedSliceRep> BlendedSliceRepPtr;

/**Slice a list of volumes using a SliceProxy.
 * The volumes are blended together during the slice process.
 * 
 * Used as the slicer for multi-layer slicing in Sonowand 2.1.
 */
class BlendedSliceRep : public RepImpl
{	
	Q_OBJECT
public:
	virtual ~BlendedSliceRep();
	static BlendedSliceRepPtr New(const std::string& uid);
	virtual std::string getType() const { return "ssc::LayerSliceRep"; };
	void setSliceProxy(SliceProxyPtr slicer); 
	void setImages(std::vector<ImagePtr> images);
	void update();
protected:
	BlendedSliceRep(const std::string& uid);
	virtual void addRepActorsToViewRenderer(View* view) ;
	virtual void removeRepActorsFromViewRenderer(View* view) ;
	void addInputImages(vtkImageDataPtr slicedImage);
	
private slots:
	void updateAlphaSlot();
//	void updateThresholdSlot(double val);
private:
	double getAlpha(int countImage);	
	bool firstImage;
	vtkImageDataPtr mBaseImages;
	SliceProxyPtr mSlicer;
	ssc::SlicedImageProxyPtr mSlicedProxy;
	std::vector<ssc::SlicedImageProxyPtr> mSlices;
	
	
	vtkImageBlendPtr mBlender;
	int countImage;
	vtkImageActorPtr mImageActor;
		
};

//---------------------------------------------------------
}//end namespace
//---------------------------------------------------------









#endif /*SSCLAYERSLICEREP_H_*/
