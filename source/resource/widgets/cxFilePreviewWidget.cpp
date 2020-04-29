/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#include <cxFilePreviewWidget.h>

#include <QTextEdit>
#include <QLabel>
#include <QTextDocument>
#include <QPushButton>
#include <QTextStream>
//#include <QFileInfo>
#include <QFile>
#include <iostream>
#include "cxTypeConversions.h"
#include <QFileSystemWatcher>
#include "cxLogger.h"
#include "snwSyntaxHighlighter.h"
#include "cxFilePreviewProperty.h"

namespace cx
{

FilePreviewWidget::FilePreviewWidget(QWidget* parent, FilePreviewPropertyPtr dataInterface, QGridLayout *gridLayout, int row) :
	FileWatcherWidget(parent, "file_preview_widget", "File Preview"),
	mTextDocument(new QTextDocument(this)),
	mTextEdit(new QTextEdit(this)),
	mSaveButton(new QPushButton("Save", this))
{
	mData = dataInterface;
	if(mData) //Allow widget to be used like before without a dataInterface
	{
		//connect(mData.get(), SIGNAL(changed()), this, SLOT(setModified()));
		connect(mData.get(), &Property::changed, this, &FilePreviewWidget::fileinterfaceChanged);
	}

	this->setToolTip("Preview and edit a file");
	mSyntaxHighlighter = NULL;
	connect(mSaveButton, SIGNAL(clicked()), this, SLOT(saveSlot()));
	mSaveButton->setEnabled(false);

	connect(mTextEdit, SIGNAL(textChanged()), this, SLOT(textChangedSlot()));
	mTextEdit->setDocument(mTextDocument);
	mTextEdit->setLineWrapMode(QTextEdit::NoWrap);

	this->setSyntaxHighLighter<snw::SyntaxHighlighter>();

	this->createAndAddEditorLayout(gridLayout, row);

	//this->setModified();//Needed?
	this->fileinterfaceChanged();
}

void FilePreviewWidget::createAndAddEditorLayout(QGridLayout *gridLayout, int row)
{
	QHBoxLayout* buttonLayout = new QHBoxLayout();
	buttonLayout->addStretch();
	buttonLayout->setMargin(0);
	buttonLayout->addWidget(mSaveButton);

	QVBoxLayout* layout;
	if (gridLayout)
		layout = new QVBoxLayout();
	else
		layout = new QVBoxLayout(this);

	layout->setMargin(0);
	layout->addWidget(mTextEdit);
	layout->addLayout(buttonLayout);

	if (gridLayout) // add to input gridlayout
		gridLayout->addLayout(layout, row, 0);
}

FilePreviewWidget::~FilePreviewWidget()
{}

void FilePreviewWidget::textChangedSlot()
{
	mSaveButton->setEnabled(mTextDocument->isModified());
}

void FilePreviewWidget::fileinterfaceChanged()
{
	if(mData)
		this->previewFileSlot(mData->getEmbeddedPath().getAbsoluteFilepath());
}
void FilePreviewWidget::previewFileSlot(const QString& absoluteFilePath)
{
	mSaveButton->setEnabled(false);

	if(!this->internalOpenNewFile(absoluteFilePath))
		return;

	QTextStream stream(mCurrentFile.get());
	QString text = stream.readAll();
	mCurrentFile->close();

	mTextDocument->setPlainText(text);
	mTextDocument->setModified(false);

	this->textChangedSlot();
}

void FilePreviewWidget::saveSlot()
{
	if(!mCurrentFile)
		return;

	if(!mCurrentFile->open(QIODevice::WriteOnly | QIODevice::Truncate))
	{
		reportWarning("Could not open file "+mCurrentFile->fileName());
		return;
	}

	mCurrentFile->write(mTextDocument->toPlainText().toLatin1());
	mCurrentFile->close();

	mTextDocument->setModified(false);

}

}//namespace cx
