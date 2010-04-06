#include "sscView.h"
#include <QtGui>

#include "vtkRenderWindow.h"

#ifdef USE_GLX_SHARED_CONTEXT
#include "sscSNWXOpenGLRenderWindow.h"
typedef SNWXOpenGLRenderWindow ViewRenderWindow;
#else
#include "vtkRenderWindow.h"
typedef vtkRenderWindow ViewRenderWindow;
#endif
#include "vtkRenderer.h"
#ifdef check
	#undef check
#endif

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
typedef vtkSmartPointer<ViewRenderWindow> ViewRenderWindowPtr;
namespace ssc
{

View::View(QWidget *parent, Qt::WFlags f) :
	ViewParent(parent, f), mRenderWindow( ViewRenderWindowPtr::New())
{
	this->SetRenderWindow(mRenderWindow);
	clear();
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

/**clear all content of the view. This ensures that props added from
 * outside the rep system also is cleared, and data not cleared with
 * RemoveAllViewProps() (added to fix problem in snw ultrasound rep,
 * data was not cleared, dont know why).
 */
void View::clear()
{
	removeReps();

	mRenderWindow->RemoveRenderer(mRenderer);
	mRenderer = vtkRendererPtr::New();
	mRenderer->SetBackground(0.0,0.0,0.0);
	mRenderWindow->AddRenderer(mRenderer);
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
	inherited::resizeEvent(event);

	QSize size = event->size();
	vtkRenderWindowInteractor* iren = mRenderWindow->GetInteractor();
	if(iren != NULL)
		iren->UpdateSize(size.width(), size.height());

    emit resized(size);
}

void View::print(std::ostream& os)
{
	Indent ind;
	printSelf(os, ind);
}

void View::printSelf(std::ostream & os, Indent indent)
{
	os << indent << "mUid: " << mUid << std::endl;
	os << indent << "mName: " << mName << std::endl;
	os << indent << "NumberOfReps: " << mReps.size() << std::endl;

	for (unsigned i=0; i<mReps.size(); ++i)
	{
		os << indent << "<Rep child " << i << ">" << std::endl;
		mReps[i]->printSelf(os, indent.stepDown());
		os << indent << "</Rep child " << i << ">" << std::endl;
	}

	if (indent.includeDetails())
	{
		os << indent << "<RenderWindow>" << std::endl;
		mRenderWindow->PrintSelf(os, indent.getVtkIndent().GetNextIndent());
		os << indent << "</RenderWindow>" << std::endl;
		os << indent << "<Renderer>" << std::endl;
		mRenderer->PrintSelf(os, indent.getVtkIndent().GetNextIndent());
		os << indent << "</Renderer>" << std::endl;
		os << indent << "<Props>" << std::endl;
		vtkPropCollection* collection = mRenderer->GetViewProps();
		collection->InitTraversal();
		vtkProp* prop = collection->GetNextProp();
		while (prop)
		{
			os << indent << indent << "<Prop>" << std::endl;
			prop->PrintSelf(os, indent.getVtkIndent().GetNextIndent().GetNextIndent());
			os << indent << indent << "</Prop>" << std::endl;
			prop = collection->GetNextProp();
		}
		os << indent << "</Props>" << std::endl;
	}
}

void View::mouseMoveEvent(QMouseEvent* event)
{
	inherited::mouseMoveEvent(event);
	emit mouseMoveSignal(event);
}

void View::mouseReleaseEvent(QMouseEvent* event)
{
	inherited::mouseReleaseEvent(event);
	emit mouseReleaseSignal(event);
}

void View::mousePressEvent(QMouseEvent* event)
{
  inherited::mousePressEvent(event);
  emit mousePressSignal(event);
}

void View::wheelEvent(QWheelEvent* event)
{
  inherited::wheelEvent(event);
  emit mouseWheelSignal(event);
}

void View::showEvent(QShowEvent* event)
{
  inherited::showEvent(event);
  emit showSignal(event);
}


} // namespace ssc
