#include "cxFileWatcherWidget.h"

#include <QFileSystemWatcher>
#include <QFileInfo>
#include "sscMessageManager.h"

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
    ssc::messageManager()->sendWarning("Could not open file "+absoluteFilePath);
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
