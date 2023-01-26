/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "catch.hpp"

#include <QMainWindow>
#include <QApplication>

#include "cxtestUtilities.h"
#include "cxImage.h"
#include "cxVtkHelperClasses.h"
#include "cxVolumeHelpers.h"
#include "cxTypeConversions.h"
#include "cxRenderWindowFactory.h"
#include "cxViewCollectionWidget.h"
#include "cxForwardDeclarations.h"
#include "cxViewWidget.h"
#include "cxMultiViewCache.h"
#include "cxViewCollectionWidgetUsingViewWidgets.h"
#include "cxLogger.h"
#include "cxDataLocations.h"
#include "cxLogicManager.h"
#include "cxSettings.h"
#include "cxViewGroupData.h"

#include <vtkRenderWindowInteractor.h>
#include <vtkTextMapper.h>
#include <vtkRenderWindow.h>
#include <vtkTextProperty.h>
#include <vtkActor2D.h>
#include <vtkRendererCollection.h>
#include <vtkImageActor.h>
#include <vtkImageData.h>
#include <vtkCamera.h>
#include <vtkSmartVolumeMapper.h>
#include <vtkVolumeProperty.h>
#include <vtkPiecewiseFunction.h>
#include <vtkGenericOpenGLRenderWindow.h>

#include <QVTKOpenGLNativeWidget.h>

//#ifndef CX_VTK_OPENGL2
//#include <vtkVolumeTextureMapper3D.h>
//#endif

namespace cxtest
{

TEST_CASE("Render volume with texture mapper and text overlay", "[integration][resource][visualization][textrender]")
{
//	QSurfaceFormat::setDefaultFormat(QVTKOpenGLNativeWidget::defaultFormat());
	vtkImageDataPtr image = cx::generateVtkImageData(Eigen::Array3i(3,3,3), Eigen::Vector3d(1, 1, 1), 100);

	vtkRenderWindowPtr renderWindow = vtkRenderWindowPtr::New();
	vtkRenderWindowInteractorPtr interactor = vtkRenderWindowInteractorPtr::New();
	interactor->SetRenderWindow(renderWindow);


	vtkRendererPtr mRenderer = vtkRendererPtr::New();
	mRenderer->SetBackground(0.1,0.4,0.2);
	mRenderer->GetActiveCamera()->SetParallelProjection(true);

	renderWindow->AddRenderer(mRenderer);
	renderWindow->SetSize(120,120);

	//Text
//	cx::TextDisplay::forceUseVtkTextMapper();
	vtkTextMapperPtr mapper = vtkTextMapperPtr::New();
	mapper->SetInput("Test");
	mapper->GetTextProperty()->SetColor(1, 0, 0);
	mapper->GetTextProperty()->SetFontSize(20);
	mapper->GetTextProperty()->SetFontFamilyToArial();

	vtkActor2DPtr textActor = vtkActor2DPtr::New();
	textActor->SetMapper(mapper);

	mRenderer->AddActor(textActor);

	//Volume
//	vtkVolumeTextureMapper3DPtr volumeMapper = vtkVolumeTextureMapper3DPtr::New();
	vtkNew<vtkSmartVolumeMapper> volumeMapper;
	volumeMapper->SetInputData(image);

	vtkVolumePtr volume = vtkVolumePtr::New();
	volume->SetMapper(volumeMapper);
	mRenderer->AddVolume(volume);

	mRenderer->ResetCamera();
	renderWindow->Render();

//#ifndef CX_VTK_OPENGL2
//	REQUIRE(volumeMapper->IsRenderSupported(volume->GetProperty(), mRenderer));
//#endif

	int numRenders = 100;
	for (int i = 0; i < numRenders; ++i)
	{
		QString text(qstring_cast("Test: ")+qstring_cast(i));
		mapper->SetInput(cstring_cast(text));
		renderWindow->Render();
	}
	REQUIRE(true);

}

TEST_CASE("vtkSmartVolumeMapper init", "[integration][resource][visualization]")
{
	//Code copied from https://kitware.github.io/vtk-examples/site/Cxx/VolumeRendering/SmartVolumeMapper/
//	vtkNew<vtkGenericOpenGLRenderWindow> renWin;//Need vtkGenericOpenGLRenderWindow instead?- Don't work yet - missing GL?
	vtkNew<vtkRenderWindow> renWin;

	vtkNew<vtkRenderer> ren1;
	CHECK(renWin);
	CHECK(ren1);
	ren1->SetBackground(0.1, 0.4, 0.2);
	renWin->AddRenderer(ren1);
	renWin->SetSize(301, 300); // intentional odd and NPOT  width/height

	vtkNew<vtkRenderWindowInteractor> iren;
	CHECK(iren);
	iren->SetRenderWindow(renWin);
	renWin->Render(); // make sure we have an OpenGL context.

	//Both crash if vtk_module_autoinit isn't run
	vtkNew<vtkSmartVolumeMapper> volumeMapper;
//	vtkSmartPointer<vtkSmartVolumeMapper> volumeMapper = vtkSmartPointer<vtkSmartVolumeMapper>::New();
	CHECK(volumeMapper);


	vtkRenderWindowInteractorPtr i = renWin->GetInteractor();
	CHECK(i);
}

TEST_CASE("vtkGenericOpenGLRenderWindow init", "[integration][resource][visualization][vkt9]")
{
	//For Qt+VTK, see: https://kitware.github.io/vtk-examples/site/Cxx/Qt/SideBySideRenderWindowsQt/
	vtkNew<vtkGenericOpenGLRenderWindow> renWin;
	CHECK(renWin);

	QVTKOpenGLNativeWidget *qvtkWidget = new QVTKOpenGLNativeWidget;
	qvtkWidget->setRenderWindow(renWin);//Fixes fail on renWin->Render() below
//	qvtkWidget->setRenderWindow(renWin.Get());

	vtkNew<vtkRenderer> ren1;
	CHECK(renWin);
	CHECK(ren1);
	ren1->SetBackground(0.1, 0.4, 0.2);
	renWin->AddRenderer(ren1);
	renWin->SetSize(301, 300);

	renWin->Render(); // Fails without qvtkWidget->setRenderWindow


	vtkNew<vtkRenderWindowInteractor> iren;
	CHECK(iren);
	iren->SetRenderWindow(renWin);
	renWin->Render(); // make sure we have an OpenGL context.
}

TEST_CASE("QVTKOpenGLNativeWidget render", "[integration][resource][visualization][hide][vkt9]")
{
	QSurfaceFormat::setDefaultFormat(QVTKOpenGLNativeWidget::defaultFormat());
	vtkImageDataPtr image = cx::generateVtkImageData(Eigen::Array3i(3,3,3), Eigen::Vector3d(1, 1, 1), 200);

	QMainWindow *mainWindow = new QMainWindow();
	CHECK(mainWindow);

	mainWindow->resize(600,500);
	mainWindow->show();

//	vtkNew<vtkGenericOpenGLRenderWindow> renderWindow;
//	vtkRenderWindowPtr renderWindow = vtkRenderWindowPtr::New();//Old render window (= vtkXOpenGLRenderWindow)

	vtkRenderWindowPtr renderWindow;
	cx::ViewPtr view;
	QWidget *widget = nullptr;

	//Use ViewCollectionWidget for creating widgets/renderwindows
	cx::RenderWindowFactoryPtr factory = cx::RenderWindowFactoryPtr(new cx::RenderWindowFactory());
	cx::ViewCollectionWidget* mLayoutWidget = cx::ViewCollectionWidget::createViewWidgetLayout(factory, mainWindow);
//	cx::ViewCollectionWidget* mLayoutWidget = cx::ViewCollectionWidget::createOptimizedLayout(factory, mainWindow);
	widget = mLayoutWidget;
	view = mLayoutWidget->addView(cx::View::VIEW_3D, cx::LayoutRegion(0,0));
//	mLayoutWidget->addView(cx::View::VIEW_3D, cx::LayoutRegion(0,1));//Adding more views works
	std::vector<cx::ViewPtr> views = mLayoutWidget->getViews();
	CHECK(views.size() > 0);
	renderWindow = view->getRenderWindow();//Fixed by using renderWindow created in QVTKOpenGLNativeWidget, instead of the one from factory


	//Test: LayoutWidgetUsingViewWidgets
//	cx::ViewCollectionWidget* layoutWidget = new cx::LayoutWidgetUsingViewWidgets(factory, mainWindow);
//	widget = layoutWidget;
//	view = layoutWidget->addView(cx::View::VIEW_3D, cx::LayoutRegion(0,0));
//	renderWindow = view->getRenderWindow();//This is probably the problem

	//Test:MultiViewCache - ok
//	cx:: MultiViewCachePtr mViewCache = cx::MultiViewCache::create(factory);
//	cx::ViewWidget* viewWidget = mViewCache->retrieveView(mainWindow, cx::View::VIEW_3D, false);//ok
//	//Test: add more code from LayoutWidgetUsingViewWidgets::addView()
//	viewWidget->getView()->setType(cx::View::VIEW_3D);
//	viewWidget->setParent(mainWindow);//Don't use this pointer?

	//Test: Create ViewWidget directly - ok
//	cx::ViewWidget* viewWidget = new cx::ViewWidget(factory, mainWindow, "testUid");//Fixed
////	cx::ViewWidget* viewWidget = new cx::ViewWidget("testUid", factory);//Test: Create an empty ViewWidget. Needs a renderWindow - This seems to work!
//	widget = viewWidget;
//	renderWindow = viewWidget->renderWindow();
//	CX_LOG_DEBUG() << "renderWindow2: " << renderWindow;


	//Test: use renderWindow from view instead if viewWidget
//	renderWindow = viewWidget->getView()->getRenderWindow();//This fails
//	CX_LOG_DEBUG() << "renderWindow3: " << renderWindow;

	//viewWidget->render();viewWidget->render();


//	//mainWindow->setCentralWidget(mLayoutWidget);//Needed to place widget correctly
	//TODO: view is still black/empty
//	renderWindow->ShowWindowOn();
//	mLayoutWidget->render();mLayoutWidget->render();//Rendering early don't cause any errors
//	renderWindow->Render();renderWindow->Render();


	//Create QVTKOpenGLNativeWidget directly - works
//	QVTKOpenGLNativeWidget *qvtkWidget = new QVTKOpenGLNativeWidget();
//	renderWindow = qvtkWidget->renderWindow();//Test: Don't create new renderwindow, just use the one automatically created in QVTKOpenGLNativeWidget - This works
//	widget = qvtkWidget;


//	qvtkWidget->setRenderWindow(renderWindow);
//	qvtkWidget->setParent(mainWindow);//NB: Needs mainWindow to work?
	mainWindow->setCentralWidget(widget);//Also work. This is the way it is done in cxMainWindow

	vtkRendererPtr mRenderer = vtkRendererPtr::New();
//	mRenderer = viewWidget->getRenderer();//Test: Use renderer from viewWidget instead of creating a new - no improvement
	mRenderer->SetBackground(0.1,0.4,0.2);
	mRenderer->GetActiveCamera()->SetParallelProjection(true);

	renderWindow->AddRenderer(mRenderer);
	renderWindow->SetSize(120,120);

	vtkNew<vtkSmartVolumeMapper> volumeMapper;
	volumeMapper->SetInputData(image);

	vtkVolumePtr volume = vtkVolumePtr::New();
	volume->SetMapper(volumeMapper);
	mRenderer->AddVolume(volume);

	mRenderer->ResetCamera();
	renderWindow->Render();
//	qvtkWidget->renderWindow()->Render();

	renderWindow->Render();

	qApp->exec();//Needs user to close app
//	cxtest::Utilities::sleep_sec(3);
}

TEST_CASE("ViewService::createLayoutWidget", "[resource][visualization][viewservice][integration][hide][vkt9]")
{
	cx::DataLocations::setTestMode();
	cx::LogicManager::initialize();

	cx::settings()->setValue("optimizedViews", false);

	//ctkPluginContext* context = cx::LogicManager::getInstance()->getPluginContext();

	QMainWindow *mainWindow = new QMainWindow();
	CHECK(mainWindow);

	mainWindow->resize(600,500);
	mainWindow->show();

	cx::ViewServicePtr viewservice = cx::logicManager()->getViewService();
	QWidget *widget = cx::logicManager()->getViewService()->createLayoutWidget(mainWindow, 0);

	QWidget* widget2 = viewservice->getLayoutWidget(0);
	CHECK(widget == widget2);

	mainWindow->setCentralWidget(widget);


	//Show something in 3D
//	cx::ViewPtr view = viewservice->get3DView(0, 0);
//	REQUIRE(view);
//	vtkRenderWindowPtr renderWindow = view->getRenderWindow();
//	REQUIRE(renderWindow);
//	vtkRendererPtr mRenderer = vtkRendererPtr::New();
//	mRenderer->SetBackground(0.1,0.4,0.2);
//	mRenderer->GetActiveCamera()->SetParallelProjection(true);
//	renderWindow->AddRenderer(mRenderer);
////	renderWindow->SetSize(120,120);
//	vtkNew<vtkSmartVolumeMapper> volumeMapper;
//	vtkImageDataPtr image = cx::generateVtkImageData(Eigen::Array3i(3,3,3), Eigen::Vector3d(1, 1, 1), 200);
//	volumeMapper->SetInputData(image);
//	vtkVolumePtr volume = vtkVolumePtr::New();
//	volume->SetMapper(volumeMapper);
//	mRenderer->AddVolume(volume);
//	mRenderer->ResetCamera();

//	viewservice->enableRender(false);//Removes GL errors, but no rendering - Stops CX renderloop


	qApp->exec();//Needs user to close app

	cx::LogicManager::shutdown();
}

} //namespace cxtest
