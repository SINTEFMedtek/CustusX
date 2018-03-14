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

namespace cx
{

FilePreviewWidget::FilePreviewWidget(QWidget* parent) :
		FileWatcherWidget(parent, "file_preview_widget", "File Preview"),
	mTextDocument(new QTextDocument(this)),
	mTextEdit(new QTextEdit(this)),
	mSaveButton(new QPushButton("Save", this))
{
	this->setToolTip("Preview and edit a file");
	mSyntaxHighlighter = NULL;
  connect(mSaveButton, SIGNAL(clicked()), this, SLOT(saveSlot()));
  mSaveButton->setEnabled(false);

  QHBoxLayout* buttonLayout = new QHBoxLayout();
  buttonLayout->addStretch();
  buttonLayout->setMargin(0);
  buttonLayout->addWidget(mSaveButton);

  QVBoxLayout* layout = new QVBoxLayout(this);
  layout->setMargin(0);
  layout->addWidget(mTextEdit);
  layout->addLayout(buttonLayout);

  connect(mTextEdit, SIGNAL(textChanged()), this, SLOT(textChangedSlot()));
  mTextEdit->setDocument(mTextDocument);
  mTextEdit->setLineWrapMode(QTextEdit::NoWrap);

  this->setSyntaxHighLighter<snw::SyntaxHighlighter>();
}

FilePreviewWidget::~FilePreviewWidget()
{}

void FilePreviewWidget::textChangedSlot()
{
	mSaveButton->setEnabled(mTextDocument->isModified());
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
