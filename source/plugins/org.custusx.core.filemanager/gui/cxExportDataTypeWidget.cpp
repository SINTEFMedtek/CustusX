/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include "cxExportDataTypeWidget.h"

#include <QVBoxLayout>
#include <QLabel>

#include "cxOptionsWidget.h"
#include "cxFileReaderWriterService.h"
#include "cxFileManagerService.h"
#include "cxLogger.h"
#include "cxImage.h"

namespace cx
{

Q_OBJECTExportDataTypeWidget::Q_OBJECTExportDataTypeWidget(QWidget *parent, QString dataType, FileManagerServicePtr filemanager, VisServicesPtr services, SelectDataStringPropertyBasePtr property) :
	BaseWidget(parent, "ExportDataTypeWidget", "Export "+dataType),
	mFileManager(filemanager),
	mServices(services),
	mSelectedData(property),
	mDataType(dataType)
{
	QStringList exporters;
	std::vector<FileReaderWriterServicePtr> writers = filemanager->getExportersForDataType(mDataType);
	CX_LOG_DEBUG() << "Found " << writers.size() << " writers that export " << mDataType;
	for(int i=0; i<writers.size(); ++i)
	{
		//exporters << writers[i]->getName()+ " (*." + writers[i]->getFileSuffix() +")";
		exporters << writers[i]->getFileSuffix();
	}

	mSelectedData->setValueName(mDataType);
	mSelectedData->setHelp("Select "+mDataType);

	//create gui
	std::vector<SelectDataStringPropertyBasePtr> properties;
	properties.push_back(mSelectedData);
	OptionsWidget *options = new OptionsWidget(services->view(), services->patient(), this);
	options->setOptions(mDataType, properties, true);

	//add the layout
	QVBoxLayout *layout = new QVBoxLayout(this);
	layout->addWidget(new QLabel(mDataType));
	QString exporter;
	foreach(exporter,exporters)
	{
		QCheckBox *checkbox = new QCheckBox(exporter);
		mCheckboxes.push_back(checkbox);
		layout->addWidget(checkbox);
	}
	mExportButton = new QPushButton("Export");
	mExportAllButton = new QPushButton("Export All "+mDataType+"s");

	QHBoxLayout *hLayout = new QHBoxLayout();
	hLayout->addWidget(options);
	hLayout->addWidget(mExportButton);
	hLayout->addWidget(new QLabel("or"));
	hLayout->addWidget(mExportAllButton);
	layout->addLayout(hLayout);
	layout->addWidget(this->createHorizontalLine());
}

void Q_OBJECTExportDataTypeWidget::exportButtonClicked()
{
	CX_LOG_ERROR() << "Q_OBJECTExportDataTypeWidget::exportButtonClicked()";
}

void Q_OBJECTExportDataTypeWidget::exportAllButtonClicked()
{
	CX_LOG_ERROR() << "Q_OBJECTExportDataTypeWidget::exportAllButtonClicked()";
}

}
