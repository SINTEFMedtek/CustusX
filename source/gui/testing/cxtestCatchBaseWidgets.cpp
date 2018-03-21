/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#include "catch.hpp"

#include "cxLogicManager.h"
#include "cxPatientModelService.h"
#include "cxViewService.h"
#include "cxVideoService.h"

//cxBaseWidget children in alphabetical order
#include "cxActiveToolWidget.h"
#include "cxClippingWidget.h"
#include "cxColorWidget.h"
#include "cxCroppingWidget.h"
//#include "cxDataSelectWidget.h" //special case
#include "cxEraserWidget.h"
#include "cxFrameTreeWidget.h"
#include "cxMetricWidget.h"
#include "cxNavigationWidget.h"
#include "cxOverlayWidget.h"
#include "cxPlaybackWidget.h"
#include "cxProbeConfigWidget.h"
#include "cxSamplerWidget.h"
#include "cxShadingWidget.h"
//#include "cxSimulateUSWidget.h"
#include "cxToolPropertiesWidget.h"
#include "cxTrackPadWidget.h"
#include "cxTransferFunction2DColorWidget.h"
#include "cxTransferFunction2DOpacityWidget.h"
#include "cxTransferFunctionWidget.h"
#include "cxTransferFunctionAlphaWidget.h"
#include "cxTransferFunctionColorWidget.h"
#include "cxTransferFunctionWidget.h"
#include "cxTransform3DWidget.h"
#include "cxVideoConnectionWidget.h"
//cxFileWatcherWidgets
#include "cxFilePreviewWidget.h"
#include "cxImagePreviewWidget.h"
//cxInfoWidgets
#include "cxMeshInfoWidget.h"
#include "cxVolumeInfoWidget.h"
//cxPresetWidgets
#include "cxTransferFunctionPresetWidget.h"
//cxTabbedWidgets
#include "cxSlicePropertiesWidget.h"
#include "cxVolumePropertiesWidget.h"
#include "cxTrackingService.h"
#include "cxPatientModelServiceProxy.h"
#include "cxtestSpaceProviderMock.h"

//All these widgets in the plugins folder should also be tested
//#include "cxAllFiltersWidget.h"
//#include "cxCompactFilterSetupWidget.h"
//#include "cxFilterWidget.h" //in plugings
//#include "cxLapFrameToolCalibrationWidget.h"
//#include "cxPipelineWidget.h"
//#include "cxPipelineWidgetFilterLine.h"
//#include "cxReconstructionWidget.h"
//#include "cxRecordSessionWidget.h"
//#include "cxRegistrationHistoryWidget.h"
//#include "cxSoundSpeedConverterWidget.h"
//#include "cxTemporalCalibrationWidget.h"
//#include "cxToolManualCalibrationWidget.h"
//#include "cxToolTipCalibrateWidget.h"
//#include "cxToolTipSampleWidget.h"
//#include "cxTrackPadWidget.h"
////cxPresetWidgets
//#include "FilterPresetWidget.h"
////cxRecordBaseWidgets
//#include "cxTrackedCenterlineWidget.h"
//#include "cxUSAcqusitionWidget.h"
////RegistrationBaseWidgets
//#include "cxElastixWidget.h"
//#include "cxFastOrientationRegistrationWidget.h"
//#include "cxLandmarkImage2ImageRegistrationWidget.h"
//#include "cxPlateImageRegistrationWidget.h"
//#include "cxFastPatientRegistrationWidget.h"
//#include "cxManualImageRegistrationWidget.h"
//#include "cxManualPatientRegistrationWidget.h"
//#include "cxPatientOrientationWidget.h"
//#include "cxPlateRegistrationWidget.h"
//#include "cxPrepareVesselsWidget.h"
//#include "cxRegisterI2IWidget.h"
//#include "cxSeansVesselRegistrationWidget.h"
//#include "cxWirePhantomWidget.h"
////cxTabbedWidgets
//#include "cxCalibrationMethodsWidget.h"
//#include "cxRegistrationMethodsWidget.h"

namespace cxtest
{

namespace
{
void testBaseWidgetChild(cx::BaseWidget* widget)
{
	REQUIRE(widget);
	CHECK_THAT(widget->objectName().toStdString(), EndsWith("widget"));
	CHECK_FALSE(widget->windowTitle().isEmpty());
}

void testAndDeleteBaseWidgetChild(cx::BaseWidget* widget)
{
	testBaseWidgetChild(widget);
	delete widget;
}

void init()
{
	cx::LogicManager::initialize();
}
void shutdown()
{
	cx::LogicManager::shutdown();
}
}

TEST_CASE("BaseWidget's children in gui/dataWidgets correctly constructed", "[unit][gui][widget][not_win32]")
{
	init();
	QWidget* testParent = new QWidget();
	cx::PatientModelServicePtr patientModelService = cx::PatientModelService::getNullObject(); //mock PatientModelService with the null object
	cx::ViewServicePtr viewService = cx::ViewService::getNullObject(); //mock
	cx::TrackingServicePtr trackingService = cx::TrackingService::getNullObject(); //mock
	cx::SpaceProviderPtr spaceProvider(new  cxtest::SpaceProviderMock()); //mock
	cx::VisServicesPtr services = cx::VisServices::getNullObjects();

	testAndDeleteBaseWidgetChild(new cx::ActiveToolWidget(trackingService, testParent));
	testAndDeleteBaseWidgetChild(new cx::ActiveVolumeWidget(patientModelService, viewService, testParent));
	testAndDeleteBaseWidgetChild(new cx::ClippingWidget(services, testParent));
	testAndDeleteBaseWidgetChild(new cx::ColorWidget(patientModelService, testParent));
	testAndDeleteBaseWidgetChild(new cx::CroppingWidget(patientModelService, viewService, testParent));
//	testAndDeleteBaseWidgetChild(new cx::DataSelectWidget(testParent));//special case: Needs a SelectDataStringPropertyBasePtr moc object
	testAndDeleteBaseWidgetChild(new cx::EraserWidget(patientModelService, viewService, testParent));
	testAndDeleteBaseWidgetChild(new cx::FrameTreeWidget(patientModelService, testParent));
	testAndDeleteBaseWidgetChild(new cx::MetricWidget(services, testParent));
	testAndDeleteBaseWidgetChild(new cx::NavigationWidget(viewService, trackingService, testParent));
	testAndDeleteBaseWidgetChild(new cx::OverlayWidget(patientModelService, viewService, testParent));
	testAndDeleteBaseWidgetChild(new cx::PlaybackWidget(trackingService, services->video(), patientModelService, testParent));
//	testAndDeleteBaseWidgetChild(new cx::ProbeConfigWidget(testParent));
	testAndDeleteBaseWidgetChild(new cx::SamplerWidget(trackingService, patientModelService, spaceProvider, testParent));
	testAndDeleteBaseWidgetChild(new cx::ShadingWidget(patientModelService->getActiveData(), testParent));
//	testAndDeleteBaseWidgetChild(new cx::SimulateUSWidget(testParent));
	testAndDeleteBaseWidgetChild(new cx::ActiveToolPropertiesWidget(trackingService, spaceProvider, testParent));
	testAndDeleteBaseWidgetChild(new cx::TrackPadWidget(viewService, testParent));
	testAndDeleteBaseWidgetChild(new cx::Transform3DWidget(testParent));


	//cxTabbedWidgets in plugins
//	testAndDeleteBaseWidgetChild(new cx::CalibrationMethodsWidget(testParent));
//	testAndDeleteBaseWidgetChild(new cx::FastRegistrationsWidget(testParent));
//	testAndDeleteBaseWidgetChild(new cx::Image2ImageRegistrationWidget(testParent));
//	testAndDeleteBaseWidgetChild(new cx::Image2PlateRegistrationWidget(testParent));
//	testAndDeleteBaseWidgetChild(new cx::LandmarkRegistrationsWidget(testParent));
//	testAndDeleteBaseWidgetChild(new cx::ManualRegistrationsWidget(testParent));
//	testAndDeleteBaseWidgetChild(new cx::RegistrationMethodsWidget(testParent));

	delete testParent;
	shutdown();
}

TEST_CASE("VideoConnectionWidget is correctly constructed", "[unit][gui][widget][not_win32]")
{
	init();
	cx::VisServicesPtr services = cx::VisServices::create(cx::logicManager()->getPluginContext());
	testAndDeleteBaseWidgetChild(new cx::VideoConnectionWidget(services, NULL));
	services.reset();
	shutdown();
}

TEST_CASE("FileWatcherWidgets are correctly constructed", "[unit][gui][widget][not_win32]")
{
	init();
	//cxFileWatcherWidgets
	testAndDeleteBaseWidgetChild(new cx::FilePreviewWidget(NULL));
	testAndDeleteBaseWidgetChild(new cx::ImagePreviewWidget(NULL));
	shutdown();
}

TEST_CASE("TabbedWidgets are correctly constructed", "[unit][gui][widget][not_win32]")
{
	init();
	ctkPluginContext *pluginContext = cx::LogicManager::getInstance()->getPluginContext();
	cx::VisServicesPtr services = cx::VisServices::create(pluginContext);
	//cxTabbedWidgets
	testAndDeleteBaseWidgetChild(new cx::SlicePropertiesWidget(services->patient(), services->view(), NULL));
	testAndDeleteBaseWidgetChild(new cx::VolumePropertiesWidget(services, NULL));
	shutdown();
}

TEST_CASE("InfoWidgets are correctly constructed", "[unit][gui][widget][not_win32]")
{
	init();
	ctkPluginContext *pluginContext = cx::LogicManager::getInstance()->getPluginContext();
	cx::VisServicesPtr services = cx::VisServices::create(pluginContext);
	cx::PatientModelServicePtr patientModelService = cx::PatientModelServiceProxy::create(pluginContext);
	cx::ViewServicePtr viewService = cx::ViewService::getNullObject(); //mock
	//cxInfoWidgets
	testAndDeleteBaseWidgetChild(new cx::VolumeInfoWidget(patientModelService, NULL));
	testAndDeleteBaseWidgetChild(new cx::ActiveMeshPropertiesWidget(services, NULL));
	shutdown();
}

TEST_CASE("TransferFunction widgets are correctly constructed", "[unit][gui][widget][not_win32]")
{
	init();
	cx::PatientModelServicePtr mockPatientModelService = cx::PatientModelService::getNullObject();
	cx::ActiveDataPtr mockActiveData = mockPatientModelService->getActiveData();

	testAndDeleteBaseWidgetChild(new cx::TransferFunction2DColorWidget(mockActiveData, NULL));
	testAndDeleteBaseWidgetChild(new cx::TransferFunction2DOpacityWidget(mockActiveData, NULL));
	testAndDeleteBaseWidgetChild(new cx::TransferFunctionWidget(mockPatientModelService, NULL));
	testAndDeleteBaseWidgetChild(new cx::TransferFunctionAlphaWidget(mockActiveData, NULL));
	testAndDeleteBaseWidgetChild(new cx::TransferFunctionColorWidget(mockActiveData, NULL));
	testAndDeleteBaseWidgetChild(new cx::TransferFunctionWidget(mockPatientModelService, NULL));
	testAndDeleteBaseWidgetChild(new cx::TransferFunction2DWidget(mockActiveData, NULL));
	testAndDeleteBaseWidgetChild(new cx::TransferFunction3DWidget(mockActiveData, NULL));
	shutdown();
}

} //namespace cxtest
