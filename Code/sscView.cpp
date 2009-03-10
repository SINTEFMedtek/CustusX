#include "sscView.h"

#include "vtkRenderWindow.h"
#include "vtkRenderer.h"
#ifdef check
	#undef check
#endif
#include <QtGui>
#include "sscRep.h"

/*! Copy/pasted from qitemdelegate.cpp
  \internal

  Note that on Mac, if /usr/include/AssertMacros.h is included prior
  to QItemDelegate, and the application is building in debug mode, the
  check(assertion) will conflict with QItemDelegate::check.

  To avoid this problem, add

  #ifdef check
	#undef check
  #endif

  after including AssertMacros.h
*/

namespace ssc
{

View::View(QWidget *parent, Qt::WFlags f) :
	QVTKWidget(parent, f), mRenderer(vtkRendererPtr::New()), mRenderWindow(vtkRenderWindowPtr::New())
{
	mRenderWindow->AddRenderer(mRenderer);
	this->SetRenderWindow(mRenderWindow);

	mRenderer->SetBackground(0.5,0.5,0.5);

	//mRenderWindow->Render();
}

View::~View()
{
}
std::string View::getUid()
{
  return mUid;
}
std::string View::getName()
{
  return mName;
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

void View::resizeEvent ( QResizeEvent * event )
{
	QSize size = event->size();
	vtkRenderWindowInteractor* iren = mRenderWindow->GetInteractor();
	if(iren != NULL)
		iren->UpdateSize(size.width(), size.height());
    emit resized(size);
}


} // namespace ssc
