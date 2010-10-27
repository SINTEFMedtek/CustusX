#ifndef SSCAXESREP_H_
#define SSCAXESREP_H_

#include <vector>
#include "sscRepImpl.h"
#include "sscTransform3D.h"
#include "vtkForwardDeclarations.h"

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

	static AxesRepPtr New(const QString& uid);

	virtual QString getType() const { return "ssc::AxesRep"; }
	void setTransform(Transform3D rMt);
	void setFontSize(double size);
	void setAxisLength(double length);
	void setVisible(bool on);

	void setShowAxesLabels(bool on);
	void setCaption(const QString& caption, const Vector3D& color);

protected:
	AxesRep(const QString& uid);
	virtual void addRepActorsToViewRenderer(View* view);
	virtual void removeRepActorsFromViewRenderer(View* view);
	void addCaption(const QString& label, Vector3D pos, Vector3D color);
	vtkAxesActorPtr mActor;
	vtkAssemblyPtr mAssembly;
	std::vector<vtkCaptionActor2DPtr> mCaption;
	std::vector<Vector3D> mCaptionPos;
	double mSize;
	double mFontSize;

};

} // namespace ssc

#endif /*SSCAXESREP_H_*/
