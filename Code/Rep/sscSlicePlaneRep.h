#ifndef SSCSLICEPLANEREP_H_
#define SSCSLICEPLANEREP_H_

#include "sscRepImpl.h"
#include <vector>
#include "sscTransform3D.h"
#include "vtkForwardDeclarations.h"
#include "sscForwardDeclarations.h"

namespace ssc
{

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

	static SlicePlaneRepPtr New(const QString& uid);

	virtual QString getType() const { return "ssc::SliceRep"; }
	void setImage(ImagePtr image);
	bool hasImage(ImagePtr image) const;
	void setSliceProxy(SliceProxyPtr proxy);
	void setSliceReps( std::vector<ssc::SliceRepSWPtr> slicerReps);
	
protected:
	SlicePlaneRep(const QString& uid);
	virtual void addRepActorsToViewRenderer(View* view);
	virtual void removeRepActorsFromViewRenderer(View* view);
	
private slots:
	void sliceTransformChangedSlot(Transform3D sMr);
	
private:
	SliceProxyPtr mSlicer;
	std::vector<SliceRepSWPtr> mSlicesPlanes;
	vtkImagePlaneWidgetPtr mVtkImagePlaneWidgetA;
	vtkImagePlaneWidgetPtr mVtkImagePlaneWidgetC;
	vtkImagePlaneWidgetPtr mVtkImagePlaneWidgetS;
	
	ImagePtr mImage;
 
};

} // namespace ssc


#endif /*SSCSLICEPLANEREP_H_*/
