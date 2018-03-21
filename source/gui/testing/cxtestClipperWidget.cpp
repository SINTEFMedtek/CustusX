/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#include "catch.hpp"
#include <QTableWidget>
#include "cxClipperWidget.h"
#include "cxVisServices.h"
#include "cxClippers.h"
#include "cxInteractiveClipper.h"
#include "cxtestSessionStorageTestFixture.h"
#include "cxPatientModelService.h"
#include "cxStringPropertySelectTool.h"
#include "cxTrackingServiceProxy.h"
#include "cxSlicePlanes3DRep.h"
#include "cxSliceProxy.h"
#include "cxtestSessionStorageHelper.h"

#include "cxLogicManager.h"

namespace cxtest
{

typedef boost::shared_ptr<class VisServicesFixture> VisServicesFixturePtr;
class VisServicesFixture : public cx::VisServices
{
public:
	void setPatient(cx::PatientModelServicePtr patient)
	{
		mPatientModelService = patient;
	}
	void setTracking(cx::TrackingServicePtr tracking)
	{
		mTrackingService = tracking;
	}
};

typedef boost::shared_ptr<class ClipperWidgetFixture> ClipperWidgetFixturePtr;
class ClipperWidgetFixture : public cx::ClipperWidget
{
public:
	ClipperWidgetFixture() :
		ClipperWidget(cxtest::VisServicesFixture::getNullObjects(), NULL)
	{
		init();
	}
	ClipperWidgetFixture(cx::VisServicesPtr services) :
		ClipperWidget(services, NULL)
	{
		init();
	}

	void init()
	{
		cx::ClippersPtr clippers = cx::ClippersPtr(new cx::Clippers(mServices));
		QString clipperName = clippers->getClipperNames().first();
		testClipper = clippers->getClipper(clipperName);

		clipperName = clippers->getClipperNames().last();
		testClipper2 = clippers->getClipper(clipperName);
	}

	std::map<QString, cx::DataPtr> getDataMap()
	{
		return this->getDatas();
	}
	QMap<QString, QCheckBox*> getCheckBoxes()
	{
		QMap<QString, QCheckBox*> checkBoxes;

		int rows = mDataTableWidget->rowCount();

		for (int row = 0; row < rows; ++row)
		{
			QTableWidgetItem *descriptionItem = mDataTableWidget->item(row, 1);
			REQUIRE(descriptionItem);
			QString name = descriptionItem->text();
			QCheckBox *checkbox = dynamic_cast<QCheckBox*>(mDataTableWidget->cellWidget(row, 0));
			REQUIRE(checkbox);
			checkBoxes[name] = checkbox;
		}

		return checkBoxes;
	}
	void forceDataStructuresUpdate()
	{
		this->prePaintEvent();
	}
	void checkUseClipperCheckBox()
	{
		this->mUseClipperCheckBox->setChecked(true);
	}
	QCheckBox *getUseClipperCheckBox()
	{
		return mUseClipperCheckBox;
	}
	QCheckBox *getShowImages()
	{
		return mShowImages;
	}
	QCheckBox *getShowMeshes()
	{
		return mShowMeshes;
	}
	QCheckBox *getSelectAllData()
	{
		return mSelectAllData;
	}
	cx::StringPropertySelectToolPtr getToolSelector()
	{
		return mToolSelector;
	}

	QCheckBox *requireGetCheckBoxForMesh1()
	{
		QString uid = testData.mesh1->getName();
		return requireGetCheckBoxForData(uid);
	}

	QCheckBox *requireGetCheckBoxForImage1()
	{
		QString uid = testData.image1->getName();
		return requireGetCheckBoxForData(uid);
	}

	QCheckBox *requireGetCheckBoxForData(QString uid)
	{
		QCheckBox *checkbox = NULL;
		int rows = mDataTableWidget->rowCount();

		for (int row = 0; row < rows; ++row)
		{
			QTableWidgetItem *descriptionItem = mDataTableWidget->item(row, 1);
			REQUIRE(descriptionItem);
			QString name = descriptionItem->text();
			if(name == uid)
			{
				checkbox = dynamic_cast<QCheckBox*>(mDataTableWidget->cellWidget(row, 0));
				break;
			}
		}
		REQUIRE(checkbox);
		return checkbox;
	}

	cx::InteractiveClipperPtr testClipper;
	cx::InteractiveClipperPtr testClipper2;

	TestDataStructures testData;
};

typedef boost::shared_ptr<class InteractiveClipperFixture> InteractiveClipperFixturePtr;
class InteractiveClipperFixture : public cx::InteractiveClipper
{
public:
	InteractiveClipperFixture(cx::CoreServicesPtr services) :
		InteractiveClipper(services)
	{}
	cx::SlicePlanesProxyPtr getSlicePlanesProxy()
	{
		return mSlicePlanesProxy;
	}
};


TEST_CASE_METHOD(cxtest::ClipperWidgetFixture, "ClipperWidget: Set clipper", "[unit][gui][widget][clip]")
{
	CHECK_FALSE(mClipper);
	this->setClipper(testClipper);
	CHECK(mClipper);
}

TEST_CASE_METHOD(cxtest::ClipperWidgetFixture, "ClipperWidget: Enable clipper", "[unit][gui][widget][clip]")
{
	this->setClipper(testClipper);
	this->mUseClipperCheckBox->setChecked(true);
	CHECK(mUseClipperCheckBox->isChecked());
	CHECK(testClipper->getUseClipper());

	this->mUseClipperCheckBox->setChecked(false);
	CHECK_FALSE(testClipper->getUseClipper());

	this->setClipper(testClipper2);
	this->mUseClipperCheckBox->setChecked(true);
	CHECK(testClipper2->getUseClipper());

	this->setClipper(testClipper);
	CHECK_FALSE(testClipper->getUseClipper());
	CHECK_FALSE(mUseClipperCheckBox->isChecked());
}

TEST_CASE("ClipperWidget: Insert data", "[unit][gui][widget][clip]")
{
	SessionStorageHelper helper;
	helper.createTestPatient();
	ClipperWidgetFixturePtr fixture = ClipperWidgetFixturePtr(new cxtest::ClipperWidgetFixture(helper.getServices()));

	cx::PatientModelServicePtr patientService = helper.storageFixture.mPatientModelService;

	CHECK(fixture->getDataMap().size() == 0);
	CHECK(fixture->getCheckBoxes().size() == 0);

	patientService->insertData(helper.testData.mesh1);
	patientService->insertData(helper.testData.image1);

	CHECK(fixture->getDataMap().size() == 2);

	fixture->setClipper(fixture->testClipper);
	CHECK(fixture->getCheckBoxes().size() == 2);

	patientService->insertData(helper.testData.image2);
	fixture->forceDataStructuresUpdate();
	CHECK(fixture->getDataMap().size() == 3);
	CHECK(fixture->getCheckBoxes().size() == 3);
}

TEST_CASE("ClipperWidget: Turn clipping on/off for a mesh", "[unit][gui][widget][clip]")
{
	SessionStorageHelper helper;
	helper.createTestPatientWithData();
	ClipperWidgetFixturePtr fixture = ClipperWidgetFixturePtr(new cxtest::ClipperWidgetFixture(helper.getServices()));

	fixture->setClipper(fixture->testClipper);
	fixture->checkUseClipperCheckBox();

	QCheckBox *checkBox = fixture->requireGetCheckBoxForMesh1();
	REQUIRE_FALSE(checkBox->isChecked());

	std::vector<vtkPlanePtr> clipPlanes = helper.testData.mesh1->getAllClipPlanes();
	REQUIRE(clipPlanes.size() == 0);

	CHECK(fixture->testClipper->getDatas().size() == 0);

	checkBox->click();
	clipPlanes = helper.testData.mesh1->getAllClipPlanes();
	CHECK(fixture->testClipper->getDatas().size() == 1);
	REQUIRE(clipPlanes.size() == 1);

	checkBox->click();
	REQUIRE_FALSE(checkBox->isChecked());
	clipPlanes = helper.testData.mesh1->getAllClipPlanes();
	CHECK(fixture->testClipper->getDatas().size() == 0);
	REQUIRE(clipPlanes.size() == 0);
}

TEST_CASE("ClipperWidget: Turn clipping on/off for an image", "[unit][gui][widget][clip]")
{
	SessionStorageHelper helper;
	helper.createTestPatientWithData();
	ClipperWidgetFixturePtr fixture = ClipperWidgetFixturePtr(new cxtest::ClipperWidgetFixture(helper.getServices()));

	fixture->setClipper(fixture->testClipper);
	QCheckBox *checkBox = fixture->requireGetCheckBoxForImage1();
	REQUIRE_FALSE(checkBox->isChecked());
	checkBox->click();

	std::vector<vtkPlanePtr> clipPlanes = helper.testData.image1->getAllClipPlanes();
	CHECK(fixture->testClipper->getDatas().size() == 1);

	checkBox->click();
	clipPlanes = helper.testData.image1->getAllClipPlanes();
	CHECK(fixture->testClipper->getDatas().size() == 0);
}

TEST_CASE("ClipperWidget: Remember clipping in mesh when changing clipper", "[unit][gui][widget][clip]")
{
	SessionStorageHelper helper;
	helper.createTestPatientWithData();
	ClipperWidgetFixturePtr fixture = ClipperWidgetFixturePtr(new cxtest::ClipperWidgetFixture(helper.getServices()));

	fixture->setClipper(fixture->testClipper);
	fixture->checkUseClipperCheckBox();

	QCheckBox *checkBox = fixture->requireGetCheckBoxForMesh1();
	checkBox->click();
	CHECK(checkBox->isChecked());

	std::vector<vtkPlanePtr> clipPlanes = helper.testData.mesh1->getAllClipPlanes();
	CHECK(clipPlanes.size() == 1);
	CHECK(fixture->testClipper->getDatas().size() == 1);

	fixture->setClipper(fixture->testClipper2);
	checkBox = fixture->requireGetCheckBoxForMesh1();
	CHECK_FALSE(checkBox->isChecked());
	clipPlanes = helper.testData.mesh1->getAllClipPlanes();
	CHECK(clipPlanes.size() == 1);
	CHECK(fixture->testClipper2->getDatas().size() == 0);

	fixture->setClipper(fixture->testClipper);

	checkBox = fixture->requireGetCheckBoxForMesh1();
	CHECK(checkBox->isChecked());
}

TEST_CASE("ClipperWidget: Data type selector can turn images on/off", "[unit][gui][widget][clip]")
{
	SessionStorageHelper helper;
	helper.createTestPatientWithData();
	ClipperWidgetFixturePtr fixture = ClipperWidgetFixturePtr(new cxtest::ClipperWidgetFixture(helper.getServices()));

	REQUIRE(fixture->getShowImages()->isChecked());
	REQUIRE(fixture->getShowMeshes()->isChecked());
	CHECK(fixture->getDataMap().size() == 3);

	fixture->getShowImages()->click();
	REQUIRE_FALSE(fixture->getShowImages()->isChecked());
	CHECK(fixture->getDataMap().size() == 1);

	fixture->getShowImages()->click();
	CHECK(fixture->getDataMap().size() == 3);
}

TEST_CASE("ClipperWidget: Select all data checks mesh checkbox", "[unit][gui][widget][clip]")
{
	SessionStorageHelper helper;
	helper.createTestPatientWithData();
	ClipperWidgetFixturePtr fixture = ClipperWidgetFixturePtr(new cxtest::ClipperWidgetFixture(helper.getServices()));

	fixture->setClipper(fixture->testClipper);

	QCheckBox *checkBox = fixture->requireGetCheckBoxForMesh1();
	CHECK_FALSE(checkBox->isChecked());

	REQUIRE_FALSE(fixture->getSelectAllData()->isChecked());
	fixture->getSelectAllData()->click();
	REQUIRE(fixture->getSelectAllData()->isChecked());
	fixture->forceDataStructuresUpdate();
	checkBox = fixture->requireGetCheckBoxForMesh1();
	CHECK(checkBox->isChecked());
}

TEST_CASE("ClipperWidget: Select all data is unchecked when mesh is unchecked", "[unit][gui][widget][clip]")
{
	SessionStorageHelper helper;
	helper.createTestPatientWithData();
	ClipperWidgetFixturePtr fixture = ClipperWidgetFixturePtr(new cxtest::ClipperWidgetFixture(helper.getServices()));

	fixture->setClipper(fixture->testClipper);

	fixture->getSelectAllData()->click();
	REQUIRE(fixture->getSelectAllData()->isChecked());

	fixture->forceDataStructuresUpdate();
	QCheckBox *checkBox = fixture->requireGetCheckBoxForMesh1();
	REQUIRE(checkBox->isChecked());
	checkBox->click();
	fixture->forceDataStructuresUpdate();
	REQUIRE_FALSE(fixture->getSelectAllData()->isChecked());
}

TEST_CASE("ClipperWidget: Select all data is updated when changing clipper", "[unit][gui][widget][clip]")
{
	SessionStorageHelper helper;
	helper.createTestPatientWithData();
	ClipperWidgetFixturePtr fixture = ClipperWidgetFixturePtr(new cxtest::ClipperWidgetFixture(helper.getServices()));

	fixture->setClipper(fixture->testClipper);
	fixture->getUseClipperCheckBox()->click();

	fixture->getSelectAllData()->click();
	CHECK(fixture->getSelectAllData()->isChecked());

	fixture->setClipper(fixture->testClipper2);

	REQUIRE_FALSE(fixture->getSelectAllData()->isChecked());

	fixture->setClipper(fixture->testClipper);
	CHECK(fixture->getSelectAllData()->isChecked());
}

TEST_CASE_METHOD(cxtest::ClipperWidgetFixture, "ClipperWidget: Attach to tool on/off", "[unit][gui][widget][clip]")
{
	this->setClipper(testClipper);
	REQUIRE(mAttachedToTool->isChecked());
	mAttachedToTool->click();
	REQUIRE_FALSE(mAttachedToTool->isChecked());
}

TEST_CASE("InteractiveClipper: Attach to tool/no tool updates all SliceProxy objects", "[unit][gui][widget][clip]")
{
	cx::LogicManager::initialize();

	cx::CoreServicesPtr services = cx::CoreServices::create(cx::logicManager()->getPluginContext());
	InteractiveClipperFixturePtr fixture = InteractiveClipperFixturePtr(new InteractiveClipperFixture(services));

	cx::ToolPtr tool;
	tool = services->tracking()->getActiveTool();
	fixture->setTool(tool);

	cx::SlicePlanesProxy::DataMap data = fixture->getSlicePlanesProxy()->getData();
	cx::SlicePlanesProxy::DataMap::iterator iter;
	for (iter = data.begin(); iter != data.end(); ++iter)
		CHECK(iter->second.mSliceProxy->getTool());

	tool = cx::ToolPtr();
	fixture->setTool(tool);

	data = fixture->getSlicePlanesProxy()->getData();
	for (iter = data.begin(); iter != data.end(); ++iter)
		CHECK_FALSE(iter->second.mSliceProxy->getTool());

	cx::LogicManager::shutdown();
}

TEST_CASE("ClipperWidget: Active tool selected as default", "[unit][gui][widget][clip]")
{
	SessionStorageHelper helper;
	ClipperWidgetFixturePtr fixture = ClipperWidgetFixturePtr(new cxtest::ClipperWidgetFixture(helper.getServices()));

	cx::ToolPtr tool = fixture->getToolSelector()->getTool();
	CHECK(tool);
	cx::ToolPtr activeTool = helper.getServices()->tracking()->getActiveTool();
	REQUIRE(tool == activeTool);
}

TEST_CASE("ClipperWidget: Select tool other than active works", "[unit][gui][widget][clip]")
{
	SessionStorageHelper helper;
	ClipperWidgetFixturePtr fixture = ClipperWidgetFixturePtr(new cxtest::ClipperWidgetFixture(helper.getServices()));

	cx::StringPropertySelectToolPtr toolSelector = fixture->getToolSelector();

	QStringList toolNames = toolSelector->getValueRange();
	REQUIRE(toolNames.size() > 1);

	QString toolName = toolNames[1];
	REQUIRE_FALSE(toolName.contains("Active"));
	toolSelector->setValue(toolName);

	cx::ToolPtr tool = toolSelector->getTool();
	CHECK(tool);
	REQUIRE(tool == helper.getServices()->tracking()->getTool(toolName));
}

TEST_CASE("ClipperWidget: Select active tool works", "[unit][gui][widget][clip]")
{
	SessionStorageHelper helper;
	ClipperWidgetFixturePtr fixture = ClipperWidgetFixturePtr(new cxtest::ClipperWidgetFixture(helper.getServices()));

	cx::StringPropertySelectToolPtr toolSelector = fixture->getToolSelector();

	QStringList toolNames = toolSelector->getValueRange();

	QString toolName = toolNames[1];
	QString activeToolName = toolNames[0];

	REQUIRE_FALSE(toolName.contains("Active"));
	REQUIRE(activeToolName.contains("Active"));

	toolSelector->setValue(toolName);
	toolSelector->setValue(activeToolName);

	cx::ToolPtr tool = toolSelector->getTool();
	cx::ToolPtr activeTool = helper.getServices()->tracking()->getActiveTool();
	CHECK(activeTool);
	REQUIRE(tool == activeTool);
}

}//cxtest