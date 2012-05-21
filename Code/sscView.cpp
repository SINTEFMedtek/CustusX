// This file is part of SSC,
// a C++ Library supporting Image Guided Therapy Applications.
//
// Copyright (C) 2008- SINTEF Medical Technology
// Copyright (C) 2008- Sonowand AS
//
// SSC is owned by SINTEF Medical Technology and Sonowand AS,
// hereafter named the owners. Each particular piece of code
// is owned by the part that added it to the library.
// SSC source code and binaries can only be used by the owners
// and those with explicit permission from the owners.
// SSC shall not be distributed to anyone else.
//
// SSC is distributed WITHOUT ANY WARRANTY; without even
// the implied warranty of MERCHANTABILITY or FITNESS FOR
// A PARTICULAR PURPOSE.
//
// See sscLicense.txt for more information.

#include "sscView.h"
#include <QtGui>
#include <vtkImageActor.h>
#include <vtkImageData.h>
#include "sscVector3D.h"
#include "vtkRenderWindow.h"
#include "sscViewRenderWindow.h"
#include "vtkRenderer.h"
#ifdef check
#undef check
#endif

#include "sscRep.h"
#include "sscTypeConversions.h"
#include "sscMessageManager.h"
#include "sscBoundingBox3D.h"
#include "sscTransform3D.h"
/* Copy/pasted from qitemdelegate.cpp
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

ViewBase::ViewBase(QWidget *parent, const QString& uid, const QString& name) : mZoomFactor(-1.0)
{
	mMTimeHash = 0;
	mBackgroundColor = QColor("black");
	mParent = parent;
	mUid = uid;
	mName = name;
}

ViewBase::~ViewBase()
{
}

View::View(QWidget *parent, Qt::WFlags f) :
	   ViewBase(this),
	   mRenderWindow(ViewRenderWindowPtr::New()) // set zoom to negative value to signify invalid.
{
	this->SetRenderWindow(mRenderWindow);
	clear();
}

View::View(const QString& uid, const QString& name, QWidget *parent, Qt::WFlags f) :
	   ViewBase(this, uid, name),
	   mRenderWindow(ViewRenderWindowPtr::New()) // set zoom to negative value to signify invalid.
{
	this->SetRenderWindow(mRenderWindow);
	clear();
}

View::~View()
{
}

QString ViewBase::getTypeString() const
{
	switch (this->getType())
	{
	case VIEW:
		return "View";
	case VIEW_2D:
		return "View2D";
	case VIEW_3D:
		return "View3D";
	case VIEW_REAL_TIME:
		return "ViewRealTime";
	}
	return "";
}

QString ViewBase::getUid()
{
	return mUid;
}

QString ViewBase::getName()
{
	return mName;
}

vtkRendererPtr ViewBase::getRenderer() const
{
	return mRenderer;
}

vtkRenderWindowPtr View::getRenderWindow() const
{
	return mRenderWindow;
}

void ViewBase::addRep(const RepPtr& rep)
{
	if (hasRep(rep))
	{
		return;
	}

	rep->connectToView(this);
	mReps.push_back(rep);
}

void ViewBase::setRep(const RepPtr& rep)
{
	removeReps();
	addRep(rep);
}

void ViewBase::setBackgoundColor(QColor color)
{
	mBackgroundColor = color;
	if (mRenderer)
	{
		mRenderer->SetBackground(mBackgroundColor.redF(), mBackgroundColor.greenF(), mBackgroundColor.blueF());
	}
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

void ViewBase::removeReps()
{

	for (RepsIter it = mReps.begin(); it != mReps.end(); ++it)
	{
		(*it)->disconnectFromView(this);
	}
	mReps.clear();
}

void ViewBase::removeRep(const RepPtr& rep)
{
	RepsIter it = std::find(mReps.begin(), mReps.end(), rep);

	if (it == mReps.end())
	{
		return;
	}

	rep->disconnectFromView(this);
	mReps.erase(it);
}

std::vector<RepPtr> ViewBase::getReps()
{
	return mReps;
}

bool ViewBase::hasRep(const RepPtr& rep) const
{
	return std::count(mReps.begin(), mReps.end(), rep);
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

	for (unsigned i = 0; i < mReps.size(); ++i)
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
	widget::mouseMoveEvent(event);
	emit mouseMoveSignal(event);
}

void View::mousePressEvent(QMouseEvent* event)
{
	// special case for CustusX: when context menu is opened, mousereleaseevent is never called.
	// this sets the render interactor in a zoom state after each menu call. This hack prevents
	// the mouse press event in this case.
	if ((this->contextMenuPolicy() == Qt::CustomContextMenu) && event->buttons().testFlag(Qt::RightButton))
		return;

	widget::mousePressEvent(event);
	emit mousePressSignal(event);
}

void View::mouseReleaseEvent(QMouseEvent* event)
{
	widget::mouseReleaseEvent(event);
	emit mouseReleaseSignal(event);
}

void View::focusInEvent(QFocusEvent* event)
{
	widget::focusInEvent(event);
	emit focusInSignal(event);
}

void View::wheelEvent(QWheelEvent* event)
{
	widget::wheelEvent(event);
	emit mouseWheelSignal(event);
}

void View::showEvent(QShowEvent* event)
{
	widget::showEvent(event);
	emit showSignal(event);
}

void ViewBase::render()
{
	// Render is called only when mtime is changed.
	// At least on MaxOS, this is not done automatically.
	unsigned long hash = 0;

	hash += this->getRenderer()->GetMTime();
	hash += this->getRenderWindow()->GetMTime();
	vtkPropCollection* props = this->getRenderer()->GetViewProps();
	props->InitTraversal();
	for (vtkProp* prop = props->GetNextProp(); prop != NULL; prop = props->GetNextProp())
	{
		vtkImageActor* imageActor = vtkImageActor::SafeDownCast(prop);
		if (imageActor && imageActor->GetInput())
		{
			hash += imageActor->GetInput()->GetMTime();
		}
		hash += prop->GetMTime();
		hash += prop->GetRedrawMTime();
	}
	if (hash != mMTimeHash)
	{
		this->getRenderWindow()->Render();
		mMTimeHash = hash;
	}
}

void View::setZoomFactor(double factor)
{
	if (similar(factor, mZoomFactor))
	{
		return;
	}
	mZoomFactor = factor;
	emit resized(this->size());
}

double ViewBase::getZoomFactor() const
{
	return mZoomFactor;
}

ssc::DoubleBoundingBox3D ViewBase::getViewport_s() const
{
	return transform(this->get_vpMs().inv(), this->getViewport());
}

Transform3D ViewBase::get_vpMs() const
{
	Vector3D center_vp = this->getViewport().center();
	double scale = mZoomFactor / this->mmPerPix();	//  double zoomFactor = 0.3; // real magnification
	Transform3D S = createTransformScale(Vector3D(scale, scale, scale));
	Transform3D T = createTransformTranslate(center_vp);// center of viewport in viewport coordinates
	Transform3D M_vp_w = T * S; // first scale , then translate to center.
	return M_vp_w;
}

/**return the pixel viewport.
 */
ssc::DoubleBoundingBox3D ViewBase::getViewport() const
{
	QSize size = mParent->size();
	return ssc::DoubleBoundingBox3D(0, size.width(), 0, size.height(), 0, 0);
}

double ViewBase::mmPerPix() const
{
	QWidget* screen = qApp->desktop()->screen(qApp->desktop()->screenNumber(mParent));
	double r_h = (double) screen->heightMM() / (double) screen->geometry().height();
	double r_w = (double) screen->widthMM() / (double) screen->geometry().width();
	double retval = (r_h + r_w) / 2.0;
	return retval;
}

} // namespace ssc
