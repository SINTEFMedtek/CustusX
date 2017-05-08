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

#include "cxFileWatcherWidget.h"
#include "cxLogger.h"
#include <QFileSystemWatcher>
#include <QFileInfo>


namespace cx
{
FileWatcherWidget::FileWatcherWidget(QWidget* parent, QString objectName, QString windowTitle) :
	BaseWidget(parent, objectName, windowTitle),
    mFileSystemWatcher(new QFileSystemWatcher())
{
  connect(mFileSystemWatcher, SIGNAL(fileChanged(const QString&)), this, SLOT(previewFileSlot(const QString&)));
}

FileWatcherWidget::~FileWatcherWidget()
{}

bool FileWatcherWidget::internalOpenNewFile(const QString absoluteFilePath)
{
  if(mCurrentFile)
  {
    if(mCurrentFile->isOpen())
    {
      this->watchFile(false);
      mCurrentFile->close();
    }
  }

  mCurrentFile.reset(new QFile(absoluteFilePath));
  this->watchFile(true);

  if(!mCurrentFile->exists())
  {
    return false;
  }

  if(!mCurrentFile->open(QIODevice::ReadOnly))
  {
    reportWarning("Could not open file "+absoluteFilePath);
    //return false; Is this missing???
  }
  return true;
}

void FileWatcherWidget::watchFile(bool on)
{
	if (!mCurrentFile)
		return;

	QFileInfo info(*mCurrentFile);

	if (on)
	{
		if (!mFileSystemWatcher->files().contains(info.absoluteFilePath()))
			mFileSystemWatcher->addPath(info.absoluteFilePath());
	}
	else
	{
		mFileSystemWatcher->removePath(info.absoluteFilePath());
	}
}
} /* namespace cx */
