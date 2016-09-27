#include "qvtkwidget3.h"

#include "vtkRenderWindowInteractor.h"
#include "vtkInteractorStyle.h"
#include "vtkInteractorStyleTrackballCamera.h"

#include <QResizeEvent>

QVTKWidget3::QVTKWidget3(QWidget *parent, Qt::WindowFlags f, QSurfaceFormat format)
	: QOpenGLWidget(parent, f)
	, m_renWin(nullptr)
{
	// VTK requires a compatibility profile
	format.setProfile(QSurfaceFormat::CompatibilityProfile);
	setFormat(format);

	// Initialize interactors
	m_irenAdapter = new QVTKInteractorAdapter(this);
	m_connect = vtkSmartPointer<vtkEventQtSlotConnect>::New();
}

// Destructor
QVTKWidget3::~QVTKWidget3()
{
  // Following line is not needed.
  // get rid of the VTK window
  // this->SetRenderWindow(NULL);
}

// GetRenderWindow
vtkGenericOpenGLRenderWindow* QVTKWidget3::GetRenderWindow()
{
	if (this->m_renWin == nullptr)
	{
		// create a default vtk window
		vtkGenericOpenGLRenderWindow* win = vtkGenericOpenGLRenderWindow::New();
		this->SetRenderWindow(win);
	}

	return this->m_renWin;
}

// SetRenderWindow
void QVTKWidget3::SetRenderWindow(vtkGenericOpenGLRenderWindow* w)
{
	// do nothing if we don't have to
	if(w == this->m_renWin) {
		return;
	}

	// unregister previous window
	if(this->m_renWin != nullptr) {
		this->m_renWin->Finalize();
		this->m_renWin->SetMapped(0);

		m_connect->Disconnect(m_renWin, vtkCommand::WindowMakeCurrentEvent, this, SLOT(MakeCurrent()));
		m_connect->Disconnect(m_renWin, vtkCommand::WindowIsCurrentEvent, this, SLOT(IsCurrent(vtkObject*, unsigned long, void*, void*)));
		m_connect->Disconnect(m_renWin, vtkCommand::WindowFrameEvent, this, SLOT(Frame()));
		m_connect->Disconnect(m_renWin, vtkCommand::StartEvent, this, SLOT(Start()));
		m_connect->Disconnect(m_renWin, vtkCommand::EndEvent, this, SLOT(End()));
		m_connect->Disconnect(m_renWin, vtkCommand::WindowIsDirectEvent, this, SLOT(IsDirect(vtkObject*, unsigned long, void*, void*)));
		m_connect->Disconnect(m_renWin, vtkCommand::WindowSupportsOpenGLEvent, this, SLOT(SupportsOpenGL(vtkObject*, unsigned long, void*, void*)));
	}

	// now set the window
	this->m_renWin = w;

	if(this->m_renWin != nullptr) {
		// if it is mapped somewhere else, unmap it
		this->m_renWin->Finalize();
		this->m_renWin->SetMapped(1);

		// tell the vtk window what the size of this window is
		this->m_renWin->SetSize(this->width(), this->height());
		this->m_renWin->SetPosition(this->x(), this->y());

		// if an interactor wasn't provided, we'll make one by default
		if(this->m_renWin->GetInteractor() == NULL)
		{
			// create a default interactor
			QVTKInteractor* iren = QVTKInteractor::New();
			iren->SetUseTDx(false);
			this->m_renWin->SetInteractor(iren);
			iren->Initialize();

			// now set the default style
			vtkInteractorStyle* s = vtkInteractorStyleTrackballCamera::New();
			iren->SetInteractorStyle(s);

			iren->Delete();
			s->Delete();
		}

		// tell the interactor the size of this window
		this->m_renWin->GetInteractor()->SetSize(this->width(), this->height());

		m_connect->Connect(m_renWin, vtkCommand::WindowMakeCurrentEvent, this, SLOT(MakeCurrent()));
		m_connect->Connect(m_renWin, vtkCommand::WindowIsCurrentEvent, this, SLOT(IsCurrent(vtkObject*, unsigned long, void*, void*)));
		m_connect->Connect(m_renWin, vtkCommand::WindowFrameEvent, this, SLOT(Frame()));
		m_connect->Connect(m_renWin, vtkCommand::StartEvent, this, SLOT(Start()));
		m_connect->Connect(m_renWin, vtkCommand::EndEvent, this, SLOT(End()));
		m_connect->Connect(m_renWin, vtkCommand::WindowIsDirectEvent, this, SLOT(IsDirect(vtkObject*, unsigned long, void*, void*)));
		m_connect->Connect(m_renWin, vtkCommand::WindowSupportsOpenGLEvent, this, SLOT(SupportsOpenGL(vtkObject*, unsigned long, void*, void*)));
	}
}

// GetInteractor
QVTKInteractor* QVTKWidget3::GetInteractor()
{
	return QVTKInteractor::SafeDownCast(this->GetRenderWindow()->GetInteractor());
}

// Initialize
void QVTKWidget3::initializeGL()
{
	if(this->m_renWin == nullptr) {
		return;
	}

	this->m_renWin->OpenGLInitContext();
}

// Paint
void QVTKWidget3::paintGL()
{
	vtkRenderWindowInteractor* iren = nullptr;
	if(this->m_renWin != nullptr) {
		iren = this->m_renWin->GetInteractor();
	}

	if(iren == nullptr || !iren->GetEnabled()) {
		return;
	}

	iren->Render();
}

// Resize
void QVTKWidget3::resizeGL(int w, int h)
{
	if(this->m_renWin == nullptr) {
		return;
	}

	this->m_renWin->SetSize(w,h);

	// and update the interactor
	if(this->m_renWin->GetInteractor() != NULL) {
		QResizeEvent e(QSize(w,h), QSize());
		m_irenAdapter->ProcessEvent(&e, this->m_renWin->GetInteractor());
	}
}

// Move
void QVTKWidget3::moveEvent(QMoveEvent* e)
{
	QWidget::moveEvent(e);

	if(this->m_renWin == nullptr) {
		return;
	}

	this->m_renWin->SetPosition(this->x(), this->y());
}


// --------
//  Slots
// --------

void QVTKWidget3::Start()
{
	makeCurrent();
	m_renWin->PushState();
	m_renWin->OpenGLInitState();
}

void QVTKWidget3::End()
{
	m_renWin->PopState();
}

void QVTKWidget3::MakeCurrent()
{
	return;
	// Automaticly handled by QOpenGLWidget
	// this->makeCurrent();
}

void QVTKWidget3::IsCurrent(vtkObject*, unsigned long, void*, void* call_data)
{
	bool* ptr = reinterpret_cast<bool*>(call_data);
	*ptr = (int)true;
}

void QVTKWidget3::IsDirect(vtkObject*, unsigned long, void*, void* call_data)
{
	int* ptr = reinterpret_cast<int*>(call_data);
	*ptr = (int)true;
}

void QVTKWidget3::SupportsOpenGL(vtkObject*, unsigned long, void*, void* call_data)
{
	int* ptr = reinterpret_cast<int*>(call_data);
	*ptr = (int)true;
}

void QVTKWidget3::Frame()
{
	if(m_renWin->GetSwapBuffers()) {
		this->update();
	}

	// This callback will call swapBuffers() for us
	// because sometimes VTK does a render without coming through this paintGL()

	// FOLLOWING NOT TESTED FOR QOPENGLWIDGET
	// if you want paintGL to always be called for each time VTK renders
	// 1. turn off EnableRender on the interactor,
	// 2. turn off SwapBuffers on the render window,
	// 3. add an observer for the RenderEvent coming from the interactor
	// 4. implement the callback on the observer to call updateGL() on this widget
	// 5. overload QVTKWidget3::paintGL() to call m_renWin->Render() instead iren->Render()
}

// ----------------------
//  Interaction handlers
// ----------------------

/*! handle mouse press event
 */
void QVTKWidget3::mousePressEvent(QMouseEvent* e)
{
	if(this->m_renWin)
	{
		m_irenAdapter->ProcessEvent(e, this->m_renWin->GetInteractor());
	}

}

/*! handle mouse move event
 */
void QVTKWidget3::mouseMoveEvent(QMouseEvent* e)
{
	if(this->m_renWin)
	{
		m_irenAdapter->ProcessEvent(e, this->m_renWin->GetInteractor());
	}
}

/*! handle enter event
 */
void QVTKWidget3::enterEvent(QEvent* e)
{
	if(this->m_renWin)
	{
		m_irenAdapter->ProcessEvent(e, this->m_renWin->GetInteractor());
	}
}

/*! handle leave event
 */
void QVTKWidget3::leaveEvent(QEvent* e)
{
	if(this->m_renWin)
	{
		m_irenAdapter->ProcessEvent(e, this->m_renWin->GetInteractor());
	}
}

/*! handle mouse release event
 */
void QVTKWidget3::mouseReleaseEvent(QMouseEvent* e)
{
	if(this->m_renWin)
	{
		m_irenAdapter->ProcessEvent(e, this->m_renWin->GetInteractor());
	}
}

/*! handle key press event
 */
void QVTKWidget3::keyPressEvent(QKeyEvent* e)
{
	if(this->m_renWin)
	{
		m_irenAdapter->ProcessEvent(e, this->m_renWin->GetInteractor());
	}
}

/*! handle key release event
 */
void QVTKWidget3::keyReleaseEvent(QKeyEvent* e)
{
	if(this->m_renWin)
	{
		m_irenAdapter->ProcessEvent(e, this->m_renWin->GetInteractor());
	}
}

void QVTKWidget3::wheelEvent(QWheelEvent* e)
{
	if(this->m_renWin)
	{
		m_irenAdapter->ProcessEvent(e, this->m_renWin->GetInteractor());
	}
}

void QVTKWidget3::contextMenuEvent(QContextMenuEvent* e)
{
	if(this->m_renWin)
	{
		m_irenAdapter->ProcessEvent(e, this->m_renWin->GetInteractor());
	}
}

void QVTKWidget3::dragEnterEvent(QDragEnterEvent* e)
{
	if(this->m_renWin)
	{
		m_irenAdapter->ProcessEvent(e, this->m_renWin->GetInteractor());
	}
}

void QVTKWidget3::dragMoveEvent(QDragMoveEvent* e)
{
	if(this->m_renWin)
	{
		m_irenAdapter->ProcessEvent(e, this->m_renWin->GetInteractor());
	}
}

void QVTKWidget3::dragLeaveEvent(QDragLeaveEvent* e)
{
	if(this->m_renWin)
	{
		m_irenAdapter->ProcessEvent(e, this->m_renWin->GetInteractor());
	}
}

void QVTKWidget3::dropEvent(QDropEvent* e)
{
	if(this->m_renWin)
	{
		m_irenAdapter->ProcessEvent(e, this->m_renWin->GetInteractor());
	}
}

bool QVTKWidget3::focusNextPrevChild(bool)
{
	return false;
}
