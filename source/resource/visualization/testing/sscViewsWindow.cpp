#include "sscViewsWindow.h"

#include <QtGui>

#include <vtkRenderer.h>
#include "vtkRenderWindow.h"
#include "vtkCamera.h"
#include "sscBoundingBox3D.h"

namespace cxtest
{

ViewsWindow::ViewsWindow()
{
	mZoomFactor = 1;
	QRect screen = qApp->desktop()->screenGeometry(qApp->desktop()->primaryScreen());
	screen.adjust(screen.width()*0.15, screen.height()*0.15, -screen.width()*0.15, -screen.height()*0.15);
	this->setGeometry(screen);
	this->setCentralWidget( new QWidget(this) );

	//gui controll
	QVBoxLayout *mainLayout = new QVBoxLayout;
	this->centralWidget()->setLayout(mainLayout);

	mSliceLayout = new QGridLayout;

	mainLayout->addLayout(mSliceLayout);//Slice layout

	mRenderingTimer = new QTimer(this);
	mRenderingTimer->start(33);
	connect(mRenderingTimer, SIGNAL(timeout()), this, SLOT(updateRender()));
}

void ViewsWindow::setDescription(const QString& desc)
{
	this->setWindowTitle(desc);
}

ViewsWindow::~ViewsWindow()
{
	mRenderingTimer->stop();
}

cx::ViewWidget* ViewsWindow::addView(QString caption, int r, int c)
{
	cx::ViewWidget* view = new cx::ViewWidget(this->centralWidget());
	this->insertView(view, caption, "", r, c);
	return view;
}

cx::ViewWidget* ViewsWindow::add2DView(QString caption, int r, int c)
{
	cx::ViewWidget* view = this->addView(caption, r, c);

	view->getRenderer()->GetActiveCamera()->ParallelProjectionOn();
	view->GetRenderWindow()->GetInteractor()->Disable();
	view->setZoomFactor(mZoomFactor);

	return view;
}

void ViewsWindow::insertView(cx::ViewWidget *view, const QString& uid, const QString& volume, int r, int c)
{
//	view->GetRenderWindow()->SetErase(false);
//	view->GetRenderWindow()->SetDoubleBuffer(false);

	QVBoxLayout *layout = new QVBoxLayout;
	mSliceLayout->addLayout(layout, r,c);

	mLayouts.push_back(view);
	layout->addWidget(view);
	layout->addWidget(new QLabel(uid+" "+volume, this));
}

cx::View* ViewsWindow::getView(int index)
{
	return mLayouts[index];
}

bool ViewsWindow::quickRunWidget()
{
	this->show();
	this->updateRender();

	return true;
}

void ViewsWindow::updateRender()
{
	for (std::vector<cx::View *>::iterator iter=mLayouts.begin(); iter!=mLayouts.end(); ++iter)
	{
		this->prettyZoom(*iter);
	}

	for (unsigned i=0; i<mLayouts.size(); ++i)
	{
		mLayouts[i]->getRenderWindow()->Render();
	}
}

void ViewsWindow::prettyZoom(cx::View *view)
{
	if (view->getZoomFactor()<0)
	{
		view->getRenderer()->ResetCamera();
		return;
	}

	cx::DoubleBoundingBox3D bb_s  = view->getViewport_s();
	double viewportHeightmm = bb_s.range()[1];//viewPortHeightPix*mmPerPix(view);
	double parallelscale = viewportHeightmm/2/view->getZoomFactor();

	vtkCamera* camera = view->getRenderer()->GetActiveCamera();
	camera->SetParallelScale(parallelscale);
}

} // namespace cxtest

