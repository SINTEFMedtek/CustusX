#ifndef SSCSLICERREPSW_H_
#define SSCSLICERREPSW_H_

#include "sscRepImpl.h"

#include "sscForwardDeclarations.h"
#include "vtkForwardDeclarations.h"

namespace ssc
{
/**Slice a volume using a SliceProxy.
 * 
 * Used as main slicer for single volume slicing in Sonowand.
 */
class SliceRepSW : public RepImpl
{
	Q_OBJECT
public:
	static SliceRepSWPtr New(const QString& uid);	
	virtual ~SliceRepSW();
	virtual QString getType() const { return "ssc::SliceRepSW"; };
	bool hasImage(ImagePtr image) const;
	void setImage(ImagePtr image);
	ImagePtr getImage();
	vtkImageActorPtr getActor();
	void setSliceProxy(ssc::SliceProxyPtr slicer);
	QString getImageUid()const;
	void update();
	virtual void printSelf(std::ostream & os, Indent indent);

protected:
	SliceRepSW(const QString& uid);
	virtual void addRepActorsToViewRenderer(View* view) ;
	virtual void removeRepActorsFromViewRenderer(View* view) ;
		
private:
	SlicedImageProxyPtr mImageSlicer;
	vtkImageActorPtr mImageActor;
};	

}// namespace ssc

#endif /*SSCSLICERREPSW_H_*/
