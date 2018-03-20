/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#include "cxFilenameWidget.h"
#include "cxFileInputWidget.h"
#include "cxFilePathProperty.h"
#include <QFileInfo>
#include <QDir>
#include "cxTypeConversions.h"
#include <iostream>
#include "cxLogger.h"

namespace cx
{

FilenameWidget::FilenameWidget(QWidget* parent, FilePathPropertyPtr dataInterface,
	QGridLayout* gridLayout, int row) :
	OptimizedUpdateWidget(parent)
{
	mData = dataInterface;
	connect(mData.get(), SIGNAL(changed()), this, SLOT(setModified()));

	QHBoxLayout* topLayout = new QHBoxLayout;
	topLayout->setMargin(0);
	this->setLayout(topLayout);

	mFileInput = new FileInputWidget(this);
	mFileInput->setUseRelativePath(false);
	connect(mFileInput, SIGNAL(fileChanged()), this, SLOT(editingFinished()));

	if (gridLayout) // add to input gridlayout
	{
		gridLayout->addWidget(mFileInput, row, 0);
	}
	else // add directly to this
	{
		topLayout->addWidget(mFileInput);
	}

	this->setModified();
}


void FilenameWidget::editingFinished()
{
	mData->setValue(mFileInput->getFilename());
}

void FilenameWidget::prePaintEvent()
{
	mFileInput->blockSignals(true);

	QString path = mData->getEmbeddedPath().getRootPath();
	if (path.isEmpty())
		path = QDir::homePath();

	mFileInput->setDescription(mData->getDisplayName());
	mFileInput->setBasePath(path);
	mFileInput->setFilename(mData->getValue());
	mFileInput->setHelp(mData->getHelp());
	mFileInput->setBrowseHelp(mData->getHelp());

	mFileInput->blockSignals(false);
}

} // namespace cx
