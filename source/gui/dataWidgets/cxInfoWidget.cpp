/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxInfoWidget.h"

#include <QGridLayout>
#include <QVBoxLayout>
#include <QTableWidget>
#include "cxImage.h"
#include "cxTypeConversions.h"

namespace cx {

InfoWidget::InfoWidget(QWidget* parent, QString objectName, QString windowTitle) :
		BaseWidget(parent, objectName, windowTitle)
{
	toptopLayout = new QVBoxLayout(this);
	toptopLayout->setMargin(0);
	gridLayout = new QGridLayout;
	gridLayout->setMargin(0);
	toptopLayout->addLayout(gridLayout);

	mTableWidget = new QTableWidget(this);
	mTableWidget->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
}

void InfoWidget::addStretch()
{
	toptopLayout->addStretch();
}

void InfoWidget::populateTableWidget(std::map<std::string, std::string>& info)
{
	mTableWidget->setRowCount(info.size());
	mTableWidget->setColumnCount(2);
	QStringList horizontalHeaders;
	horizontalHeaders << "Description";
	horizontalHeaders << "Value";
	mTableWidget->setHorizontalHeaderLabels(horizontalHeaders);
	mTableWidget->horizontalHeaderItem(0)->setTextAlignment(Qt::AlignLeft);
	mTableWidget->horizontalHeaderItem(1)->setTextAlignment(Qt::AlignLeft);
	mTableWidget->setColumnWidth(0, 200);
	mTableWidget->setColumnWidth(1, 600);

	QFont boldFont;
	boldFont.setBold(true);

	int row = 0;
	std::map<std::string, std::string>::iterator it;
	for(it = info.begin(); it != info.end(); ++it)
	{
		QTableWidgetItem *descriptionItem = new QTableWidgetItem(qstring_cast(it->first));
		descriptionItem->setFont(boldFont);
		descriptionItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
		mTableWidget->setItem(row, 0, descriptionItem);

		QTableWidgetItem *valueItem = new QTableWidgetItem(qstring_cast(it->second));
		valueItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
		mTableWidget->setItem(row, 1, valueItem);

		row++;
	}
}

} /* namespace cx */
