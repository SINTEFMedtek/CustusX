/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors
   may be used to endorse or promote products derived from this software
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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

namespace cxtest
{

struct SelectClippersForDataWidgetFixture : cx::SelectClippersForDataWidget
{
    SelectClippersForDataWidgetFixture(cx::VisServicesPtr services) :
        cx::SelectClippersForDataWidget(services, NULL)
    {}

	QMap<QString, QCheckBox*> getDataCheckBoxes()
	{
		QMap<QString, QCheckBox*> checkBoxes;

		int rows = mClipperTableWidget->rowCount();

		for (int row = 0; row < rows; ++row)
		{
			QTableWidgetItem *descriptionItem = mClipperTableWidget->item(row, 1);
			REQUIRE(descriptionItem);
			QString name = descriptionItem->text();
			QCheckBox *checkbox = dynamic_cast<QCheckBox*>(mClipperTableWidget->cellWidget(row, 0));
			REQUIRE(checkbox);
			checkBoxes[name] = checkbox;
		}

		return checkBoxes;
	}
	void forceDataStructuresUpdate()
	{
		this->prePaintEvent();
	}

};

struct SelectClippersForDataWidgetHelper
{
	SelectClippersForDataWidgetHelper()
    {
        fixture = new SelectClippersForDataWidgetFixture(sessionHelper.getServices());
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

TEST_CASE("SelectClippersForDataWidget: Using checkboxes updates clippers", "[unit][gui][widget][clip]")
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

TEST_CASE("SelectClippersForDataWidget: CheckBoxes are updated when clipper changes", "[unit][gui][widget][clip]")
{
	SelectClippersForDataWidgetHelper helper;

	cx::DataPtr activeData = helper.getActiveData();
	cx::InteractiveClipperPtr clipper = helper.getTestClipper();
	QString clipperName = helper.getTestClipperName();
	QMap<QString, QCheckBox*> dataCheckBoxes = helper.fixture->getDataCheckBoxes();

	QCheckBox *dataCheckBox = dataCheckBoxes[clipperName];

	REQUIRE_FALSE(dataCheckBox->isChecked());
	clipper->addData(activeData);

	helper.fixture->forceDataStructuresUpdate();
	dataCheckBoxes = helper.fixture->getDataCheckBoxes();
	dataCheckBox = dataCheckBoxes[clipperName];
	REQUIRE(dataCheckBox->isChecked());
}


}//cxtest
