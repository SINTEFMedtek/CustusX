/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#include "catch.hpp"
#include <QCheckBox>
#include <QTableWidget>
#include "cxSelectClippersForDataWidget.h"
#include "cxLogicManager.h"
#include "cxVisServices.h"
#include "cxViewService.h"
#include "cxPatientModelService.h"
#include "cxClippers.h"
#include "cxInteractiveClipper.h"
#include "cxActiveData.h"
#include "cxtestSessionStorageHelper.h"
#include "cxSelectDataStringProperty.h"

namespace cxtest
{

struct SelectClippersForDataWidgetFixture : cx::SelectClippersForDataWidget
{
    SelectClippersForDataWidgetFixture(cx::VisServicesPtr services) :
		cx::SelectClippersForDataWidget(services, NULL)
	{
	}

	QMap<QString, QCheckBox*> getDataCheckBoxes()
	{
		return this->getCheckBoxes(0);
	}
	QMap<QString, QCheckBox*> getInvertCheckBoxes()
	{
		return this->getCheckBoxes(2);
	}
	QMap<QString, QCheckBox*> getCheckBoxes(int column)
	{
		QMap<QString, QCheckBox*> checkBoxes;

		int rows = mClipperTableWidget->rowCount();

		for (int row = 0; row < rows; ++row)
		{
			QTableWidgetItem *descriptionItem = mClipperTableWidget->item(row, 1);
			REQUIRE(descriptionItem);
			QString name = descriptionItem->text();
			QCheckBox *checkbox = dynamic_cast<QCheckBox*>(mClipperTableWidget->cellWidget(row, column));
			REQUIRE(checkbox);
			checkBoxes[name] = checkbox;
		}

		return checkBoxes;
	}

	void requireActiveData()
	{
		REQUIRE_FALSE(mServices->patient()->isNull());
		REQUIRE_FALSE(mActiveDataProperty->getValue().isNull());
		CHECK(mActiveDataProperty->getData());
		REQUIRE(mServices->patient()->getData(mActiveDataProperty->getValue()));
	}

	void forceDataStructuresUpdate()
	{
		this->prePaintEvent();
//		std::cout << "active data: " << mActiveDataProperty->getValue().toStdString() << std::endl;
	}

};

struct SelectClippersForDataWidgetHelper
{
	SelectClippersForDataWidgetHelper()
    {
        fixture = new SelectClippersForDataWidgetFixture(sessionHelper.getServices());
		sessionHelper.createTestPatientWithData();
    }
    ~SelectClippersForDataWidgetHelper()
    {}


    cx::DataPtr getActiveData()
    {
        cx::ImagePtr image = sessionHelper.testData.image1;

        sessionHelper.getServices()->patient()->getActiveData()->setActive(image);
        cx::DataPtr activeData = sessionHelper.getServices()->patient()->getActiveData()->getActive<cx::Data>();
        REQUIRE(activeData);
        REQUIRE(activeData->getUid() == image->getUid());
        return activeData;
    }

    cx::InteractiveClipperPtr getTestClipper()
    {
        cx::ClippersPtr clippers = sessionHelper.getServices()->view()->getClippers();
        cx::InteractiveClipperPtr clipper = clippers->getClipper(this->getTestClipperName());
        return clipper;
    }

    QString getTestClipperName()
    {
        cx::ClippersPtr clippers = sessionHelper.getServices()->view()->getClippers();
        QStringList clipperNames = clippers->getClipperNames();
        QString clipperName = clipperNames.first();
        return clipperName;
    }


    SelectClippersForDataWidgetFixture* fixture;
    SessionStorageHelper sessionHelper;
};

TEST_CASE("SelectClippersForDataWidget: Init", "[unit][gui][widget][clip]")
{
	SelectClippersForDataWidgetHelper helper;
	REQUIRE(helper.fixture->getDataCheckBoxes().size() >= 6);
}

TEST_CASE("SelectClippersForDataWidget: Using data checkboxes updates clippers", "[unit][gui][widget][clip]")
{
	SelectClippersForDataWidgetHelper helper;

	cx::InteractiveClipperPtr clipper = helper.getTestClipper();
	QString clipperName = helper.getTestClipperName();
	cx::DataPtr activeData = helper.getActiveData();

	QMap<QString, QCheckBox*> dataCheckBoxes = helper.fixture->getDataCheckBoxes();
	QCheckBox *dataCheckBox = dataCheckBoxes[clipperName];
	REQUIRE_FALSE(clipper->exists(activeData));
	dataCheckBox->click();
	REQUIRE(clipper->exists(activeData));
	dataCheckBox->click();
	REQUIRE_FALSE(clipper->exists(activeData));
}

TEST_CASE("SelectClippersForDataWidget: Using invert checkboxes updates clippers", "[unit][gui][widget][clip]")
{
	SelectClippersForDataWidgetHelper helper;

	cx::InteractiveClipperPtr clipper = helper.getTestClipper();
	QString clipperName = helper.getTestClipperName();

	QMap<QString, QCheckBox*> invertCheckBoxes = helper.fixture->getInvertCheckBoxes();
	QCheckBox *invertCheckBox = invertCheckBoxes[clipperName];
	REQUIRE_FALSE(clipper->getInvertPlane());
	invertCheckBox->click();
	REQUIRE(clipper->getInvertPlane());
	invertCheckBox->click();
	REQUIRE_FALSE(clipper->getInvertPlane());
}

TEST_CASE("SelectClippersForDataWidget: Data checkboxes are updated when clipper changes", "[unit][gui][widget][clip]")
{
	SelectClippersForDataWidgetHelper helper;

	cx::DataPtr activeData = helper.getActiveData();
	REQUIRE(activeData);
	std::cout << "active data: " << activeData->getUid().toStdString() << std::endl;
	cx::InteractiveClipperPtr clipper = helper.getTestClipper();
	QString clipperName = helper.getTestClipperName();
	QMap<QString, QCheckBox*> dataCheckBoxes = helper.fixture->getDataCheckBoxes();

	QCheckBox *dataCheckBox = dataCheckBoxes[clipperName];

	REQUIRE_FALSE(dataCheckBox->isChecked());
	clipper->addData(activeData);

	helper.fixture->forceDataStructuresUpdate();
	helper.fixture->requireActiveData();

	dataCheckBoxes = helper.fixture->getDataCheckBoxes();
	dataCheckBox = dataCheckBoxes[clipperName];
	REQUIRE(dataCheckBox->isChecked());
}


}//cxtest
