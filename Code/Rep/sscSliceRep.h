#ifndef SSCSLICEREP_H_
#define SSCSLICEREP_H_

#include "vtkSmartPointer.h"
typedef	vtkSmartPointer<class vtkImagePlaneWidget> vtkImagePlaneWidgetPtr;

#include "sscRepImpl.h"
#include "sscImage.h"

namespace ssc
{
typedef boost::shared_ptr<class SliceRep> SliceRepPtr;

/**Represenation for one Image.
 * Use this to slice an image.
 * 
 * NOT IN USE BY ANYONE! 
 * If you want to use this class, test is thoroughly first!!
 */
class SliceRep : public RepImpl
{
public:
	virtual ~SliceRep();

	static SliceRepPtr New(const std::string& uid);

	virtual std::string getType() const { return "ssc::SliceRep"; }
	void setImage(ImagePtr image);
	//void addImage(ImagePtr image);
	//void removeImage(ImagePtr image);
	bool hasImage(ImagePtr image) const;
	
protected:
	SliceRep(const std::string& uid);
	virtual void addRepActorsToViewRenderer(View* view);
	virtual void removeRepActorsFromViewRenderer(View* view);


	vtkImagePlaneWidgetPtr mVtkImagePlaneWidget;

	ImagePtr mImage;
};

} // namespace ssc

#endif /*SSCSLICEREP_H_*/
