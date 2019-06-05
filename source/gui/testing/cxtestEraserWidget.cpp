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

#include "cxEraserWidget.h"

namespace cxtest
{

namespace
{

class EraserWidgetTest : public cx::EraserWidget
{
	public:

	EraserWidgetTest(cx::PatientModelServicePtr patientModelService, cx::ViewServicePtr viewService, QWidget* parent) :
		cx::EraserWidget(patientModelService, viewService, parent)
	{
	}

	void callRemoveSlot()
	{
		cx::EraserWidget::removeSlot();
	}

	void callDuplicateSlot()
	{
		cx::EraserWidget::duplicateSlot();
	}

	void callSaveSlot()
	{
		cx::EraserWidget::saveSlot();
	}

};

EraserWidgetTest* createNewEraserWidget() //Must be deleted by caller
{
	cx::PatientModelServicePtr patientModelService = cx::PatientModelService::getNullObject(); //mock PatientModelService with the null object
	cx::ViewServicePtr viewService = cx::ViewService::getNullObject(); //mock
	EraserWidgetTest* eraserWidget = new EraserWidgetTest(patientModelService, viewService, nullptr);
	return eraserWidget;
}

}

TEST_CASE("EraserWidget: Remove slot, test no crash for empty patient", "[unit][gui][widget]")
{
	EraserWidgetTest* eraserWidget = createNewEraserWidget();
	eraserWidget->callRemoveSlot();
	CHECK(true);
	delete eraserWidget;
}

TEST_CASE("EraserWidget: Duplicate slot, test no crash for empty patient", "[unit][gui][widget]")
{
	EraserWidgetTest* eraserWidget = createNewEraserWidget();
	eraserWidget->callDuplicateSlot();
	CHECK(true);
	delete eraserWidget;
}

TEST_CASE("EraserWidget: Save slot, test no crash for empty patient", "[unit][gui][widget]")
{
	EraserWidgetTest* eraserWidget = createNewEraserWidget();
	eraserWidget->callSaveSlot();
	CHECK(true);
	delete eraserWidget;
}

}//namespace cxtest
