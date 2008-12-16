#ifndef SSCAXESREP_H_
#define SSCAXESREP_H_

#include "vtkSmartPointer.h"
typedef vtkSmartPointer<class vtkAxesActor> vtkAxesActorPtr;

#include "sscRepImpl.h"

namespace ssc
{
typedef boost::shared_ptr<class AxesRep> AxesRepPtr;

/**Represenation for one Mesh.
 * Use this to render geometric poly data in a 3D scene.
 */
class AxesRep : public RepImpl
{
public:
	virtual ~AxesRep();

	static AxesRepPtr New(const std::string& uid);

	virtual std::string getType() const { return "ssc::AxesRep"; }

protected:
	AxesRep(const std::string& uid);
	virtual void addRepActorsToViewRenderer(View* view);
	virtual void removeRepActorsFromViewRenderer(View* view);

	vtkAxesActorPtr mActor;

};

} // namespace ssc

#endif /*SSCAXESREP_H_*/
