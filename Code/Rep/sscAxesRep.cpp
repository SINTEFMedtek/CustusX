#include "sscAxesRep.h"

#include <vtkAxesActor.h>
#include <vtkRenderer.h>

#include "sscView.h"

namespace ssc
{

AxesRep::AxesRep(const std::string& uid) :
	RepImpl(uid)
{

	mActor = vtkAxesActorPtr::New();
	mActor->SetTotalLength( 150, 150, 150 );

}

AxesRep::~AxesRep()
{
	// ??
}

AxesRepPtr AxesRep::create(const std::string& uid)
{
	AxesRepPtr retval(new AxesRep(uid));
	retval->mSelf = retval;
	return retval;
}

void AxesRep::addRepActorsToViewRenderer(View* view)
{
	view->getRenderer()->AddActor(mActor);
}

void AxesRep::removeRepActorsFromViewRenderer(View* view)
{
	view->getRenderer()->RemoveActor(mActor);
}

} // namespace ssc
