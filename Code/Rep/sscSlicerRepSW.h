#ifndef SSCSLICERREPSW_H_
#define SSCSLICERREPSW_H_

#include <vtkSmartPointer.h>

#include "sscTool.h"
#include "sscRepImpl.h"
#include "sscImage.h"
#include "sscSliceComputer.h"
#include "sscVector3D.h"
#include "sscTransform3D.h"

typedef vtkSmartPointer<class vtkWindowLevelLookupTable> vtkWindowLevelLookupTablePtr;
typedef vtkSmartPointer<class vtkImageMapToWindowLevelColors> vtkImageMapToWindowLevelColorsPtr;
typedef vtkSmartPointer<class vtkImageActor> vtkImageActorPtr;

namespace ssc
{
typedef boost::shared_ptr<class SliceRepSW> SliceRepSWPtr;

class SliceRepSW : public RepImpl
{
	Q_OBJECT
public:
	static SliceRepSWPtr New(const std::string& uid);
	
	virtual ~SliceRepSW();
	virtual std::string getType() const { return "ssc::SliceRep"; }
	bool hasImage(ImagePtr image) const;
	void setImage(ImagePtr image);
	void setTool(ToolPtr image);
	void setOrientation(SliceComputer::PLANE_TYPE orientation );
	void setFollowType(SliceComputer::FOLLOW_TYPE followType);
	
protected:
	SliceRepSW(const std::string& uid);
	virtual void addRepActorsToViewRenderer(View* view) ;
	virtual void removeRepActorsFromViewRenderer(View* view) ;

private slots:
	void updateToolTransform( Transform3D matrix, double timestamp );
	
private:
	void compute();	
	void doSliceing();
	void setMatrixData ( const Vector3D& ivec, const Vector3D& jvec, const Vector3D& center );
	void setLookupTable ();
	
	//typedef boost::shared_ptr<SliceComputer> SliceComputerPtr;
	SliceComputer mCutplane;
	SliceComputer::PLANE_TYPE mPlaneType;
	SliceComputer::FOLLOW_TYPE mFollowType;
	ImagePtr mImage;
	ToolPtr mTool;
		
	vtkWindowLevelLookupTablePtr mLookupTable;
	vtkImageMapToWindowLevelColorsPtr mWindowLevel;
	vtkImageReslicePtr mReslicer;
	vtkMatrix4x4Ptr mMatrixAxes;
	vtkImageActorPtr mImageActor;
};	

}// namespace ssc

#endif /*SSCSLICERREPSW_H_*/
