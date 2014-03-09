#include "sscViewsWindow.h"


#include <iostream>
#include <string>
#include <vector>
#include <sstream>

#include <vtkImageData.h>
#include <vtkMetaImageReader.h>
#include <vtkImagePlaneWidget.h>
#include <vtkRenderer.h>
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkCamera.h"
#include "vtkLookupTable.h"

#include "cxtestUtilities.h"
#include "sscDataManagerImpl.h"
#include "sscImage.h"
#include "sscAxesRep.h"
#include "sscImageTF3D.h"
#include "sscVolumetricRep.h"
#include "sscSliceComputer.h"
#include "sscVector3D.h"
#include "sscTransform3D.h"
#include "sscToolRep3D.h"
#include "sscDummyToolManager.h"
#include "sscDummyTool.h"
#include "sscSliceProxy.h"
#include "sscSlicerRepSW.h"
#include "sscTexture3DSlicerRep.h"
#include "sscViewsWindow.h"
#include "sscImageTF3D.h"
#include "cxDataLocations.h"
#include "cxtestRenderTester.h"
#include "sscLogger.h"
#include "cxViewsFixture.h"

#include "catch.hpp"

using cx::Vector3D;
using cx::Transform3D;


namespace cxtest
{

ViewsWindow::ViewsWindow()
{
//	mServices = cxtest::TestServices::create();
//	mMessageListener = cx::MessageListener::create();

//	this->setDescription(displayText);
	mZoomFactor = 1;
//	mShaderFolder = cx::DataLocations::getShaderPath();
	QRect screen = qApp->desktop()->screenGeometry(qApp->desktop()->primaryScreen());
	screen.adjust(screen.width()*0.15, screen.height()*0.15, -screen.width()*0.15, -screen.height()*0.15);
	this->setGeometry(screen);
	this->setCentralWidget( new QWidget(this) );

//	// Initialize dummy toolmanager.
//	mServices->trackingService()->configure();
//	mServices->trackingService()->initialize();
//	mServices->trackingService()->startTracking();

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
//	mServices.reset();
//	CHECK(!mMessageListener->containsErrors());
}

cx::ViewWidget* ViewsWindow::addView(QString caption, int r, int c)
{
//	cx::ViewWidget* view = fixture.addView("empty", 0, 0);
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

//cx::ViewWidget* ViewsWindow::create2DView(const QString& title, int r, int c)
//{
//	cx::ViewWidget* view = new cx::ViewWidget(centralWidget());

//	view->getRenderer()->GetActiveCamera()->ParallelProjectionOn();
//	view->GetRenderWindow()->GetInteractor()->Disable();
//	view->setZoomFactor(mZoomFactor);

//	return view;
//}

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

//	for (unsigned i=0; i<mLayouts.size(); ++i)
//	{
//		mLayouts[i]->getRenderWindow()->Render();
//	}

//	if (mRemaindingRenderings>=0)
//	{
//		--mRemaindingRenderings;
//		if (mRemaindingRenderings<0)
//			QTimer::singleShot(0, qApp, SLOT(quit()));
//	}
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

