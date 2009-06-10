#ifndef SSCSLICERREPSW_H_
#define SSCSLICERREPSW_H_

#include <vtkSmartPointer.h>
#include "sscDefinitions.h"
#include "sscForwardDeclarations.h"
#include "sscTool.h"
#include "sscRepImpl.h"
#include "sscImage.h"
#include "sscSliceComputer.h"
#include "sscVector3D.h"
#include "sscTransform3D.h"

#include "sscSlicedImageProxy.h"

typedef vtkSmartPointer<class vtkWindowLevelLookupTable> vtkWindowLevelLookupTablePtr;
typedef vtkSmartPointer<class vtkLookupTable> vtkLookupTablePtr;
typedef vtkSmartPointer<class vtkImageMapToWindowLevelColors> vtkImageMapToWindowLevelColorsPtr;
typedef vtkSmartPointer<class vtkImageMapToColors> vtkImageMapToColorsPtr;
typedef vtkSmartPointer<class vtkImageActor> vtkImageActorPtr;
typedef vtkSmartPointer<class vtkImageData> vtkImageDataPtr;
typedef vtkSmartPointer<class vtkImageBlend> vtkImageBlendPtr;


namespace ssc
{
typedef boost::shared_ptr<class SliceRepSW> SliceRepSWPtr;

/**Slice a volume using a SliceProxy.
 * 
 * Used as main slicer for single volume slicing in Sonowand.
 */
class SliceRepSW : public RepImpl
{
	Q_OBJECT
public:
	static SliceRepSWPtr New(const std::string& uid);	
	virtual ~SliceRepSW();
	virtual std::string getType() const { return "ssc::SliceRepSW"; };
	bool hasImage(ImagePtr image) const;
	void setImage(ImagePtr image);
	ImagePtr getImage();
	vtkImageActorPtr getActor();
	void setSliceProxy(ssc::SliceProxyPtr slicer);
	std::string getImageUid()const;
	void update();
	virtual void printSelf(std::ostream & os, Indent indent);

protected:
	SliceRepSW(const std::string& uid);
	virtual void addRepActorsToViewRenderer(View* view) ;
	virtual void removeRepActorsFromViewRenderer(View* view) ;
		
private:
	SlicedImageProxyPtr mImageSlicer;
	vtkImageActorPtr mImageActor;
};	

}// namespace ssc

#endif /*SSCSLICERREPSW_H_*/
