/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "catch.hpp"
#include <QApplication>
#include <QHeaderView>
#include <QScrollBar>
#include <QTableWidget>
#include <QVBoxLayout>
#include "cxImportWidget.h"
#include "cxImportDataTypeWidget.h"
#include "cxVisServices.h"
#include "cxFileManagerService.h"
#include "cxImage.h"
#include "cxtestUtilities.h"

namespace
{
struct ImportDataTypeWidgetVariables
{
	ImportDataTypeWidgetVariables(std::vector<cx::DataPtr> datas):
		mDatas(datas)
	{
		mImportwidget = new cx::ImportWidget(mFleManager, mVisServices);
	}
	std::vector<cx::DataPtr> mDatas;
	std::vector<cx::DataPtr> mParentCandidates;
	cx::FileManagerServicePtr mFleManager = cx::FileManagerService::getNullObject();
	cx::VisServicesPtr mVisServices = cx::VisServices::getNullObjects();
	cx::ImportWidget* mImportwidget = nullptr;
	QString mFileName;
	cx::IMAGE_MODALITY mModalitySuggestion = cx::imMR;
};

class ImportDataTypeWidgetTest : public cx::ImportDataTypeWidget
{
public:
	ImportDataTypeWidgetTest(ImportDataTypeWidgetVariables variables) :
		cx::ImportDataTypeWidget(variables.mImportwidget, variables.mVisServices, variables.mDatas, variables.mParentCandidates, variables.mFileName, variables.mModalitySuggestion)
	{}

	void setModality(cx::ImagePtr image, cx::IMAGE_MODALITY modalitySuggestion, cx::IMAGE_SUBTYPE subtype = cx::istUNKNOWN)
	{
		return cx::ImportDataTypeWidget::setModality(image, modalitySuggestion, subtype);
	}
};

}

TEST_CASE("ImportWidget", "[unit]")
{
	cx::FileManagerServicePtr fileManager = cx::FileManagerService::getNullObject();
	cx::VisServicesPtr visServices = cx::VisServices::getNullObjects();
	cx::ImportWidget* widget = new cx::ImportWidget(fileManager, visServices);
	REQUIRE(widget);

	delete widget;
}

TEST_CASE("ImportWidget::shouldWarnNoDICOMFound() only warns for the USB import action", "[unit]")
{
	CHECK_FALSE(cx::ImportWidget::shouldWarnNoDICOMFound(false, false));
	CHECK_FALSE(cx::ImportWidget::shouldWarnNoDICOMFound(false, true));
	CHECK(cx::ImportWidget::shouldWarnNoDICOMFound(true, false));
	CHECK_FALSE(cx::ImportWidget::shouldWarnNoDICOMFound(true, true));
}

TEST_CASE("ImportDataTypeWidget::setModality", "[unit]")
{
	std::vector<cx::DataPtr> datas;
	cx::ImagePtr image = cxtest::Utilities::create3DImage(Eigen::Array3i(3,3,3), 200);
	CHECK(image->getModality() == cx::imUNKNOWN);
	datas.push_back(image);

	ImportDataTypeWidgetVariables variables(datas);
	ImportDataTypeWidgetTest* widget = new ImportDataTypeWidgetTest(variables);
	REQUIRE(widget);

	CHECK(image->getModality() == cx::imMR);//Init of ImportDataTypeWidgetTest change modality to MR

	image->setModality(cx::imCOUNT);
	widget->setModality(image, cx::imPET);
	CHECK(image->getModality() == cx::imPET);

	image->setModality(cx::imUNKNOWN);
	widget->setModality(image, cx::imCT, cx::istTHORAX_CT);
	CHECK(image->getModality() == cx::imCT);
	CHECK(image->getImageType() == cx::istTHORAX_CT);

	delete widget;
}

TEST_CASE("ImportDataTypeWidget::fitQTableWidgetHeightToRows leaves room for a horizontal scrollbar", "[unit][gui][widget]")
{
	QWidget container;
	container.setFixedWidth(150);
	QVBoxLayout* layout = new QVBoxLayout(&container);
	QTableWidget* table = new QTableWidget(1, 2);
	table->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
	table->verticalHeader()->setVisible(false);
	table->setItem(0, 0, new QTableWidgetItem("1"));
	table->setItem(0, 1, new QTableWidgetItem(QString(100, 'x')));
	layout->addWidget(table);
	layout->addStretch();

	cx::ImportDataTypeWidget::fitQTableWidgetHeightToRows(table);
	container.show();
	qApp->processEvents();
	table->verticalScrollBar()->setValue(table->verticalScrollBar()->maximum());
	qApp->processEvents();

	REQUIRE(table->horizontalScrollBar()->isVisible());
	QRect lastRow = table->visualRect(table->model()->index(0, 1));
	CHECK(lastRow.bottom() < table->viewport()->height());
}
