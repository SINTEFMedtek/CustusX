#ifndef SSCAXESREP_H_
#define SSCAXESREP_H_

#include <vector>
#include "sscRepImpl.h"
#include <vtkSmartPointer.h>
typedef vtkSmartPointer<class vtkAxesActor> vtkAxesActorPtr;
typedef vtkSmartPointer<class vtkAssembly> vtkAssemblyPtr;
typedef vtkSmartPointer<class vtkCaptionActor2D> vtkCaptionActor2DPtr;
#include "sscTransform3D.h"

namespace ssc
{
typedef boost::shared_ptr<class AxesRep> AxesRepPtr;

/**Representation for one 3D coordinate axis.
 * Set the position using setTransform().
 */
class AxesRep : public RepImpl
{
public:
	virtual ~AxesRep();

	static AxesRepPtr New(const std::string& uid);

	virtual std::string getType() const { return "ssc::AxesRep"; }
	void setTransform(Transform3D rMt);
	void setFontSize(double size);
	void setAxisLength(double length);

protected:
	AxesRep(const std::string& uid);
	virtual void addRepActorsToViewRenderer(View* view);
	virtual void removeRepActorsFromViewRenderer(View* view);
	void addCaption(const std::string& label, Vector3D pos);
	vtkAxesActorPtr mActor;
	vtkAssemblyPtr mAssembly;
	std::vector<vtkCaptionActor2DPtr> mCaption;
	std::vector<Vector3D> mCaptionPos;
	double mSize;
	double mFontSize;

};

} // namespace ssc

#endif /*SSCAXESREP_H_*/
