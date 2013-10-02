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
#include "sscDataManager.h"
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
#include "sscGPURayCastVolumeRep.h"
#include "sscViewsWindow.h"
#include "sscImageTF3D.h"
#include "cxDataLocations.h"
#include "cxtestRenderTester.h"
#include "sscLogger.h"

using cx::Vector3D;
using cx::Transform3D;


//---------------------------------------------------------
//---------------------------------------------------------

//---------------------------------------------------------
//---------------------------------------------------------

namespace {
	cx::DummyToolPtr dummyTool()
	{
		return boost::dynamic_pointer_cast<cx::DummyTool>(cx::ToolManager::getInstance()->getDominantTool());
	}
}


vtkLookupTablePtr getCreateLut(int tableRangeMin, int tableRangeMax, double hueRangeMin, double hueRangeMax,
	double saturationRangeMin, double saturationRangeMax, double valueRangeMin, double valueRangeMax)
{
	vtkLookupTablePtr lut = vtkLookupTablePtr::New();
	lut->SetTableRange(tableRangeMin, tableRangeMax);
	lut->SetHueRange(hueRangeMin, hueRangeMax);
	lut->SetSaturationRange(saturationRangeMin, saturationRangeMax);
	lut->SetValueRange(valueRangeMin, valueRangeMax);
	lut->Build();

	return lut;
}

ViewsWindow::ViewsWindow(QString displayText)
{
	this->setDescription(displayText);
	mZoomFactor = 1;
	mShaderFolder = cx::DataLocations::getShaderPath();
	
	QRect screen = qApp->desktop()->screenGeometry(qApp->desktop()->primaryScreen());
	screen.adjust(screen.width()*0.15, screen.height()*0.15, -screen.width()*0.15, -screen.height()*0.15);
	this->setGeometry(screen);
	this->setCentralWidget( new QWidget(this) );

	// Initialize dummy toolmanager.
	cx::ToolManager* mToolmanager = cx::DummyToolManager::getInstance();
	mToolmanager->configure();
	mToolmanager->initialize();
	mToolmanager->startTracking();

//	cx::ToolPtr tool = mToolmanager->getDominantTool();
//	connect( tool.get(), SIGNAL( toolTransformAndTimestamp(Transform3D ,double) ), &mTimer, SLOT( start()));

	//gui controll
	QVBoxLayout *mainLayout = new QVBoxLayout;
	this->centralWidget()->setLayout(mainLayout);

	mSliceLayout = new QGridLayout;

	mainLayout->addLayout(mSliceLayout);//Slice layout

//	QHBoxLayout *controlLayout = new QHBoxLayout;
//	controlLayout->addStretch();

//	mAcceptanceBox = new cx::AcceptanceBoxWidget(mDisplayText, this);
//	controlLayout->addWidget(mAcceptanceBox);

//	controlLayout->addStretch();
//	mainLayout->addLayout(controlLayout); //Buttons

	mRenderingTimer = new QTimer(this);
	mRenderingTimer->start(33);
	connect(mRenderingTimer, SIGNAL(timeout()), this, SLOT(updateRender()));
}

void ViewsWindow::setDescription(const QString& desc)
{
	this->setWindowTitle(desc);
//	mAcceptanceBox->setText(desc);
}

ViewsWindow::~ViewsWindow()
{
	mRenderingTimer->stop();
	cx::ToolManager::shutdown();
}


bool ViewsWindow::defineGPUSlice(const QString& uid, const QString& imageFilename, cx::PLANE_TYPE plane, int r, int c)
{	
	cx::ViewWidget* view = this->create2DView(imageFilename, r, c);
	cx::ImagePtr image = loadImage(imageFilename);

	if (!view || !view->getRenderWindow() || !view->getRenderWindow()->SupportsOpenGL())
		return false;
	if (!cx::Texture3DSlicerRep::isSupported(view->getRenderWindow()))
		return false;

	cx::SliceProxyPtr proxy = this->createSliceProxy(plane);
	cx::Texture3DSlicerRepPtr rep = cx::Texture3DSlicerRep::New(uid);
	rep->setShaderFile(mShaderFolder + "/Texture3DOverlay.frag");
	rep->setSliceProxy(proxy);
	rep->setImages(std::vector<cx::ImagePtr>(1, image));
	view->addRep(rep);
	insertView(view, uid, imageFilename, r, c);

	return true;
}

void ViewsWindow::defineSlice(const QString& uid, const QString& imageFilename, cx::PLANE_TYPE plane, int r, int c)
{
	cx::ViewWidget* view = this->create2DView(imageFilename, r, c);
	cx::ImagePtr image = loadImage(imageFilename);

	cx::SliceProxyPtr proxy = this->createSliceProxy(plane);
	cx::SliceRepSWPtr rep = cx::SliceRepSW::New(uid);
	rep->setImage(image);
	rep->setSliceProxy(proxy);
	view->addRep(rep);
	insertView(view, uid, imageFilename, r, c);
}

cx::ViewWidget* ViewsWindow::create2DView(const QString& title, int r, int c)
{
	cx::ViewWidget* view = new cx::ViewWidget(centralWidget());

	view->getRenderer()->GetActiveCamera()->ParallelProjectionOn();
	view->GetRenderWindow()->GetInteractor()->Disable();
	view->setZoomFactor(mZoomFactor);

	return view;
}

cx::SliceProxyPtr ViewsWindow::createSliceProxy(cx::PLANE_TYPE plane)
{
	cx::ToolManager* mToolmanager = cx::DummyToolManager::getInstance();
	cx::ToolPtr tool = mToolmanager->getDominantTool();

	cx::SliceProxyPtr proxy(new cx::SliceProxy());
	proxy->setTool(tool);
	proxy->initializeFromPlane(plane, false, Vector3D(0,0,-1), false, 1, 0);
	return proxy;
}

cx::ImagePtr ViewsWindow::loadImage(const QString& imageFilename)
{
	QString filename = cxtest::Utilities::getDataRoot(imageFilename);
	cx::ImagePtr image = cx::DataManager::getInstance()->loadImage(filename, filename, cx::rtMETAIMAGE);
	Vector3D center = image->boundingBox().center();
	center = image->get_rMd().coord(center);
	cx::DataManager::getInstance()->setCenter(center);
	
	if (!image)
		return cx::ImagePtr();

	// side effect: set tool movement box to data box,
	dummyTool()->setToolPositionMovementBB(transform(image->get_rMd(), image->boundingBox()));
	this->fixToolToCenter();

	return image;
}

void ViewsWindow::fixToolToCenter()
{
	Vector3D c = cx::DataManager::getInstance()->getCenter();
	dummyTool()->setToolPositionMovement(std::vector<Transform3D>(1, cx::createTransformTranslate(c)));
}

void ViewsWindow::insertView(cx::ViewWidget *view, const QString& uid, const QString& volume, int r, int c)
{
	QVBoxLayout *layout = new QVBoxLayout;
	mSliceLayout->addLayout(layout, r,c);

	mLayouts.push_back(view);
	layout->addWidget(view);
	layout->addWidget(new QLabel(uid+" "+volume, this));
}

void ViewsWindow::define3D(const QString& imageFilename, const ImageParameters* parameters, int r, int c)
{
	QString uid = "3D";
	cx::ViewWidget* view = new cx::ViewWidget(centralWidget());
	
	cx::ImagePtr image = loadImage(imageFilename);
	this->applyParameters(image, parameters);

	// volume rep
	cx::VolumetricRepPtr mRepPtr = cx::VolumetricRep::New( image->getUid() );
	mRepPtr->setMaxVolumeSize(10*1000*1000);
	mRepPtr->setUseGPUVolumeRayCastMapper(); // if available
	mRepPtr->setImage(image);	
	mRepPtr->setName(image->getName());
	view->addRep(mRepPtr);
		
	insertView(view, uid, imageFilename, r, c);
}

//void ViewsWindow::showTool()
//{
//	// Tool 3D rep
//	cx::ToolManager* mToolmanager = cx::DummyToolManager::getInstance();
//	cx::ToolPtr tool = mToolmanager->getDominantTool();
//	cx::ToolRep3DPtr toolRep = cx::ToolRep3D::New( tool->getUid(), tool->getName() );
//	toolRep->setTool(tool);
//	view->addRep(toolRep);
//}

bool ViewsWindow::define3DGPU(const QStringList& imageFilenames, const ImageParameters* parameters, int r, int c)
{
	QString uid = "3D";
	cx::ViewWidget* view = new cx::ViewWidget(centralWidget());

	std::vector<cx::ImagePtr> images;

	for (int i = 0; i < imageFilenames.size(); ++i)
	{
		cx::ImagePtr image = loadImage(imageFilenames[i]);
		if (!image)
			return false;

		this->applyParameters(image, &parameters[i]);
		images.push_back(image);
	}

	// volume rep
#ifndef WIN32
	if (!cx::GPURayCastVolumeRep::isSupported(view->getRenderWindow()))
		return false;
	cx::GPURayCastVolumeRepPtr mRepPtr = cx::GPURayCastVolumeRep::New( images[0]->getUid() );
	mRepPtr->setShaderFolder(mShaderFolder);
	mRepPtr->setImages(images);
	mRepPtr->setName(images[0]->getName());
	view->addRep(mRepPtr);
#endif //WIN32
	
	insertView(view, uid, imageFilenames[0], r, c);
	return true;
}

void ViewsWindow::applyParameters(cx::ImagePtr image, const ImageParameters *parameters)
{
	if (!parameters)
		return;
	image->getTransferFunctions3D()->setLLR(parameters->llr);
	image->getTransferFunctions3D()->setAlpha(parameters->alpha);
	image->getTransferFunctions3D()->setLut(parameters->lut);
}

void ViewsWindow::updateRender()
{
	for (std::vector<cx::View *>::iterator iter=mLayouts.begin(); iter!=mLayouts.end(); ++iter)
	{
		this->prettyZoom(*iter);
	}

	for (unsigned i=0; i<mLayouts.size(); ++i)
		mLayouts[i]->getRenderWindow()->Render();
}

void ViewsWindow::prettyZoom(cx::View *view)
{
	if (view->getZoomFactor()<0)
		return;

	cx::DoubleBoundingBox3D bb_s  = view->getViewport_s();
	double viewportHeightmm = bb_s.range()[1];//viewPortHeightPix*mmPerPix(view);
	double parallelscale = viewportHeightmm/2/view->getZoomFactor();

	vtkCamera* camera = view->getRenderer()->GetActiveCamera();
	camera->SetParallelScale(parallelscale);
}

void ViewsWindow::dumpDebugViewToDisk(QString text, int viewIndex)
{
	cxtest::RenderTesterPtr renderTester = cxtest::RenderTester::create(mLayouts[viewIndex]->getRenderWindow());
	vtkImageDataPtr output = renderTester->getImageFromRenderWindow();
	renderTester->printFractionOfVoxelsAboveZero(text, output);
}

double ViewsWindow::getFractionOfBrightPixelsInView(int viewIndex, int threshold)
{
	cxtest::RenderTesterPtr renderTester = cxtest::RenderTester::create(mLayouts[viewIndex]->getRenderWindow());
	vtkImageDataPtr output = renderTester->renderToImage();
	return cxtest::Utilities::getFractionOfVoxelsAboveThreshold(output, threshold);
	//	std::cout << "numNonZeroPixels: " << numNonZeroPixels << std::endl;
}

bool ViewsWindow::runWidget()
{
	return this->runWidget(3000);
}

bool ViewsWindow::quickRunWidget()
{
	this->show();
	this->updateRender();
	return true;
}

/**show/render/execute input widget.
 * Return success of execution.
 */
bool ViewsWindow::runWidget(int duration)
{
	this->show();
#ifdef __APPLE__ // needed on mac for bringing to front: does the opposite on linux
	this->activateWindow();
#endif
	this->raise();
	this->updateRender();

	QTimer::singleShot(duration, qApp, SLOT(quit()));
	return !qApp->exec();
}

