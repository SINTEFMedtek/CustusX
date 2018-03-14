/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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
