#include "sscView.h"
#include <QtGui>
#include <vtkImageActor.h>
#include <vtkImageData.h>

#include "vtkRenderWindow.h"

#ifdef USE_GLX_SHARED_CONTEXT
#include "sscSNWXOpenGLRenderWindow.h"
typedef SNWXOpenGLRenderWindow ViewRenderWindow;
typedef vtkSmartPointer<SNWXOpenGLRenderWindow> ViewRenderWindowPtr;
#else
#include "vtkRenderWindow.h"
typedef vtkRenderWindow ViewRenderWindow;
typedef vtkSmartPointer<ViewRenderWindow> ViewRenderWindowPtr;
#endif
#include "vtkRenderer.h"
#ifdef check
	#undef check
#endif

#include "sscRep.h"
#include "sscTypeConversions.h"
#include "sscMessageManager.h"
#include "sscBoundingBox3D.h"
#include "sscTransform3D.h"
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
	ViewParent(parent, f), mRenderWindow( ViewRenderWindowPtr::New()), mZoomFactor(-1.0) // set zoom to negative value to signify invalid.
{
  mMTimeHash = 0;
  mBackgroundColor = QColor("black");
	this->SetRenderWindow(mRenderWindow);
	clear();
}

View::View(const QString& uid, const QString& name, QWidget *parent, Qt::WFlags f) :
  ViewParent(parent, f), mRenderWindow( ViewRenderWindowPtr::New()), mZoomFactor(-1.0) // set zoom to negative value to signify invalid.
{
  mMTimeHash = 0;
  mBackgroundColor = QColor("black");
  this->SetRenderWindow(mRenderWindow);
  clear();

  mUid = uid;
  mName = name;
//  this->setContextMenuPolicy(Qt::CustomContextMenu);
}

View::~View()
{
}

QString View::getTypeString() const
{
  switch(this->getType())
  {
  case VIEW: return "View";
  case VIEW_2D: return "View2D";
  case VIEW_3D: return "View3D";
  case VIEW_REAL_TIME: return "ViewRealTime";
  }
  return "";
}


QString View::getUid()
{
  return mUid;
}
QString View::getName()
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

void View::setBackgoundColor(QColor color)
{
  mBackgroundColor = color;
  this->clear();
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
	mRenderer->SetBackground(mBackgroundColor.redF(), mBackgroundColor.greenF(), mBackgroundColor.blueF());
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
    //std::cout << "resize " << getName() << " " << this->getRenderWindow()->GetMTime() << std::endl;
    //this->getRenderWindow()->Modified();
    //std::cout << "   resized " << getName() << " " << this->getRenderWindow()->GetMTime() << std::endl;
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

void View::mousePressEvent(QMouseEvent* event)
{
  // special case for CustusX: when context menu is opened, mousereleaseevent is never called.
  // this sets the render interactor in a zoom state after each menu call. This hack prevents
  // the mouse press event in this case.
  if ((this->contextMenuPolicy()==Qt::CustomContextMenu) && event->buttons().testFlag(Qt::RightButton))
    return;

  inherited::mousePressEvent(event);
  emit mousePressSignal(event);
}

void View::mouseReleaseEvent(QMouseEvent* event)
{
	inherited::mouseReleaseEvent(event);
	emit mouseReleaseSignal(event);
}
void View::focusInEvent(QFocusEvent* event)
{
  inherited::focusInEvent(event);
  emit focusInSignal(event);
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

void View::render()
{
  // Render is called only when mtime is changed.
  // At least on MaxOS, this is not done automatically.

  unsigned long hash = 0;

  hash += this->getRenderer()->GetMTime();
  hash += this->getRenderWindow()->GetMTime();
  vtkPropCollection* props = this->getRenderer()->GetViewProps();
  props->InitTraversal();
  for (vtkProp* prop = props->GetNextProp(); prop!=NULL; prop=props->GetNextProp())
  {
//    vtkProp3D* prop3D = vtkProp3D::SafeDownCast(prop);
//    if (prop3D)
//      hash += prop3D->GetMTime();
    vtkImageActor* imageActor = vtkImageActor::SafeDownCast(prop);
    if (imageActor && imageActor->GetInput())
    {
//      hash += imageActor->GetMTime();
      hash += imageActor->GetInput()->GetMTime();
    }


//    vtkVolume* volume = vtkVolume::SafeDownCast(prop);
//    if (volume)
//    {
////      std::cout << getName() << "\t" << prop->GetRedrawMTime() << "  " << prop->GetMTime() << std::endl;
//    }
    //std::cout << "--" << getName() << "\t" << hash << std::endl;
    hash += prop->GetMTime();
    hash += prop->GetRedrawMTime();
  }
//  std::cout << "--" << getName() << "\t" << hash << std::endl;

  if ( hash!=mMTimeHash )
  {
    this->getRenderWindow()->Render();
    mMTimeHash = hash;
//    std::cout << getName() << "\t" << mTime << " " << mTime_W << std::endl;
//    std::cout << "RENDER " << getName() << "\t" << hash << std::endl;
  }
}


void View::setZoomFactor(double factor)
{
  if (similar(factor, mZoomFactor))
    return;

  mZoomFactor = factor;
  emit resized(this->size());
}

double View::getZoomFactor() const
{
  return mZoomFactor;
}

ssc::DoubleBoundingBox3D View::getViewport_s() const
{
  return transform(this->get_vpMs().inv(), this->getViewport());
}

Transform3D View::get_vpMs() const
{
  Vector3D center_vp = this->getViewport().center();

//  double zoomFactor = 0.3; // real magnification
  double scale = mZoomFactor/this->mmPerPix();
  Transform3D S = createTransformScale(Vector3D(scale, scale, scale));
  Transform3D T = createTransformTranslate(center_vp);// center of viewport in viewport coordinates
  Transform3D M_vp_w = T*S; // first scale , then translate to center.

//  std::cout << "vpMs " <<  M_vp_w << std::endl;

  return M_vp_w;
}

/**return the pixel viewport.
 */
ssc::DoubleBoundingBox3D View::getViewport() const
{
  QSize size = this->size();
  ssc::DoubleBoundingBox3D vp(0, size.width(), 0, size.height(), 0, 0);
//  std::cout << "vp " << vp << std::endl;
  return vp;
}

double View::mmPerPix() const
{
  QWidget* screen = qApp->desktop()->screen(qApp->desktop()->screenNumber(this));
  double r_h = (double)screen->heightMM()/(double)screen->geometry().height();
  double r_w = (double)screen->widthMM()/(double)screen->geometry().width();
  double retval = (r_h+r_w)/2.0;
  return retval;
}


} // namespace ssc
