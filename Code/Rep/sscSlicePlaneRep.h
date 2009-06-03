#ifndef SSCSLICEPLANEREP_H_
#define SSCSLICEPLANEREP_H_

#include "vtkSmartPointer.h"
typedef	vtkSmartPointer<class vtkImagePlaneWidget> vtkImagePlaneWidgetPtr;

#include "sscSlicerRepSW.h"
#include "sscRepImpl.h"
#include "sscImage.h"
#include "sscSliceProxy.h"
namespace ssc
{
typedef boost::shared_ptr<class SlicePlaneRep> SlicePlaneRepPtr;

/**A slicer implementation. 
 * Unknown functionality.
 * 
 * Used only as experimental feature in Sonowand.
 */
class SlicePlaneRep : public RepImpl
{
	Q_OBJECT
public:
	virtual ~SlicePlaneRep();

	static SlicePlaneRepPtr New(const std::string& uid);

	virtual std::string getType() const { return "ssc::SliceRep"; }
	void setImage(ImagePtr image);
	bool hasImage(ImagePtr image) const;
	void setSliceProxy(SliceProxyPtr proxy);
	void setSliceReps( std::vector<ssc::SliceRepSWPtr> slicerReps);
	
protected:
	SlicePlaneRep(const std::string& uid);
	virtual void addRepActorsToViewRenderer(View* view);
	virtual void removeRepActorsFromViewRenderer(View* view);
	
private slots:
	void sliceTransformChangedSlot(Transform3D sMr);
	
private:
	SliceProxyPtr mSlicer;
	std::vector<ssc::SliceRepSWPtr> mSlicesPlanes;
	vtkImagePlaneWidgetPtr mVtkImagePlaneWidgetA;
	vtkImagePlaneWidgetPtr mVtkImagePlaneWidgetC;
	vtkImagePlaneWidgetPtr mVtkImagePlaneWidgetS;
	
	ImagePtr mImage;
 
};

} // namespace ssc


#endif /*SSCSLICEPLANEREP_H_*/
