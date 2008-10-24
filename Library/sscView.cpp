#include "sscView.h"

#include "vtkRenderWindow.h"
#include "vtkRenderer.h"
#include "sscRep.h"

namespace ssc
{

View::View(QWidget *parent, Qt::WFlags f) :
	QVTKWidget(parent, f), mRenderer(vtkRendererPtr::New()), mRenderWindow(vtkRenderWindowPtr::New())
{
	mRenderWindow->AddRenderer(mRenderer);
	this->SetRenderWindow(mRenderWindow);

	mRenderer->SetBackground(0.0, 0.0, 0.0);
	//mRenderWindow->Render();
}

View::~View()
{
}

vtkRendererPtr View::getRenderer() const
{
	return mRenderer;
}

vtkRenderWindowPtr View::getRenderWindow() const
{
	return mRenderWindow;
}

void View::addRep(const RepPtr& rep)
{
	if (hasRep(rep))
	{
		return;
	}
	
	rep->connectToView(this);
	mReps.push_back(rep);
	//mRenderWindow->Render();
}

void View::setRep(const RepPtr& rep)
{
	removeReps();
	addRep(rep);
}

void View::removeReps()
{
	for (RepsIter it=mReps.begin(); it != mReps.end(); ++it)
	{
		(*it)->disconnectFromView(this);
	}
	mReps.clear();
}

void View::removeRep(const RepPtr& rep)
{
	RepsIter it = std::find(mReps.begin(), mReps.end(), rep);

	if (it == mReps.end())
	{
		return;
	}

	rep->disconnectFromView(this);
	mReps.erase(it);
	//mRenderWindow->Render();
}

std::vector<RepPtr> View::getReps()
{
	return mReps;
}

bool View::hasRep(const RepPtr& rep) const
{
	return std::count(mReps.begin(), mReps.end(), rep);
}

} // namespace ssc
