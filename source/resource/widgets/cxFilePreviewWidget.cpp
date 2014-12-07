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

#include <cxFilePreviewWidget.h>

#include <QTextEdit>
#include <QLabel>
#include <QTextDocument>
#include <QPushButton>
#include <QTextStream>
//#include <QFileInfo>
#include <iostream>
#include "cxTypeConversions.h"
#include <QFileSystemWatcher>
#include "cxReporter.h"
#include "snwSyntaxHighlighter.h"

namespace cx
{

FilePreviewWidget::FilePreviewWidget(QWidget* parent) :
		FileWatcherWidget(parent, "FilePreviewWidget", "File Preview"),
	mTextDocument(new QTextDocument(this)),
	mTextEdit(new QTextEdit(this)),
	mSaveButton(new QPushButton("Save", this))
{
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

QString FilePreviewWidget::defaultWhatsThis() const
{
  return "<html>"
      "<h3>File preview.</h3>"
      "<p>Lets you preview and edit a file.</p>"
      "<p><i>Click save to keep your changes.</i></p>"
      "</html>";
}

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
