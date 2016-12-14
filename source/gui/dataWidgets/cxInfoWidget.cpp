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
