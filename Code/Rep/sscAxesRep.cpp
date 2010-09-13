#include "sscAxesRep.h"

#include <vtkAxesActor.h>
#include <vtkRenderer.h>
#include <vtkMatrix4x4.h>
#include <vtkCaptionActor2D.h>
#include <vtkTextProperty.h>
#include <vtkAssembly.h>

#include "sscView.h"

namespace ssc
{

AxesRep::AxesRep(const std::string& uid) :
	RepImpl(uid)
{
	mAssembly = vtkAssemblyPtr::New();
	mActor = vtkAxesActorPtr::New();
	mAssembly->AddPart(mActor);
	setAxisLength(50);

	this->setShowAxesLabels(true);
	setTransform(Transform3D());
	setFontSize(0.04);
}

void AxesRep::setVisible(bool on)
{
	mAssembly->SetVisibility(on);
	for (unsigned i=0; i<mCaption.size(); ++i)
		mCaption[i]->SetVisibility(on);
}

void AxesRep::setShowAxesLabels(bool on)
{
	if (on)
	{
		this->addCaption("x", Vector3D(1,0,0), Vector3D(1,0,0));
		this->addCaption("y", Vector3D(0,1,0), Vector3D(0,1,0));
		this->addCaption("z", Vector3D(0,0,1), Vector3D(0,0,1));
	}
	else
	{
		mCaption.clear();
		mCaptionPos.clear();
	}
}

void AxesRep::setCaption(const std::string& caption, const Vector3D& color)
{
	this->addCaption(caption, Vector3D(0,0,0), color);
}

/**set font size to a fraction of the normalized viewport.
 *
 */
void AxesRep::setFontSize(double size)
{
	mFontSize = size;

	for (unsigned i=0; i<mCaption.size(); ++i)
	{
		//mCaption[i]->SetWidth(mFontSize);
		mCaption[i]->SetHeight(mFontSize);
	}
}

/**set axis length to a world length
 *
 */
void AxesRep::setAxisLength(double length)
{
	mSize = length;
	mActor->SetTotalLength( mSize, mSize, mSize );
	setTransform(Transform3D(mAssembly->GetUserMatrix()));
}

/**Set the position of the axis.
 *
 */
void AxesRep::setTransform(Transform3D rMt)
{
	mAssembly->SetUserMatrix(rMt.matrix());

	for (unsigned i=0; i<mCaption.size(); ++i)
	{
		Vector3D pos = rMt.coord(mSize*mCaptionPos[i]);
		mCaption[i]->SetAttachmentPoint(pos.begin());
	}
}

void AxesRep::addCaption(const std::string& label, Vector3D pos, Vector3D color)
{
	vtkCaptionActor2DPtr cap = vtkCaptionActor2DPtr::New();
	cap->SetCaption(label.c_str());
	cap->GetCaptionTextProperty()->SetColor(color.begin());
	cap->LeaderOff();
	cap->BorderOff();
	cap->GetCaptionTextProperty()->ShadowOff();
	mCaption.push_back(cap);
	mCaptionPos.push_back(pos);
}

AxesRep::~AxesRep()
{
	// ??
}

AxesRepPtr AxesRep::New(const std::string& uid)
{
	AxesRepPtr retval(new AxesRep(uid));
	retval->mSelf = retval;
	return retval;
}

void AxesRep::addRepActorsToViewRenderer(View* view)
{
	view->getRenderer()->AddActor(mAssembly);
	for (unsigned i=0; i<mCaption.size(); ++i)
		view->getRenderer()->AddActor(mCaption[i]);
}

void AxesRep::removeRepActorsFromViewRenderer(View* view)
{
	view->getRenderer()->RemoveActor(mAssembly);
	for (unsigned i=0; i<mCaption.size(); ++i)
		view->getRenderer()->RemoveActor(mCaption[i]);
}

} // namespace ssc
