/*
 * sscFileSelectWidget.cpp
 *
 *  Created on: May 6, 2011
 *      Author: christiana
 */

#include <sscFileSelectWidget.h>
#include <QtGui>

namespace ssc
{

FileSelectWidget::FileSelectWidget(QWidget* parent)
{
  mNameFilters << "*.mhd";

  QHBoxLayout* dataLayout = new QHBoxLayout(this);
  mDataComboBox = new QComboBox(this);
  connect(mDataComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(currentDataComboIndexChanged(int)));

  mSelectDataAction = new QAction(QIcon(":/icons/open.png"), tr("&Select data"), this);
  connect(mSelectDataAction, SIGNAL(triggered()), this, SLOT(selectData()));
  mSelectDataButton = new QToolButton(this);
  mSelectDataButton->setDefaultAction(mSelectDataAction);

  dataLayout->addWidget(mDataComboBox);
  dataLayout->addWidget(mSelectDataButton);
}

QString FileSelectWidget::getFilename() const
{
  return mFilename;
}

void FileSelectWidget::setFilename(QString name)
{
  mFilename = name;

  if (QFileInfo(mFilename).isDir())
  {
    mRootPath = QFileInfo(mFilename).dir().absolutePath();
  }

  this->updateComboBox();
}

void FileSelectWidget::setNameFilter(QStringList filter)
{
  mNameFilters = filter;
}

void FileSelectWidget::setPath(QString path)
{
  mRootPath = path;
  this->updateComboBox();
}

void FileSelectWidget::selectData()
{
  QString filename = QFileDialog::getOpenFileName( this,
                                  QString(tr("Select data file")),
                                  mRootPath,
                                  tr("USAcq (*.mhd)"));

  if (filename.isEmpty())
    return;

  mFilename = filename;
  emit fileSelected(mFilename);
}

QStringList FileSelectWidget::getAllFiles(QString folder)
{
  QDir dir(folder);
  QStringList files = dir.entryList(mNameFilters, QDir::Files);


  QStringList retval;
  for (int i=0; i<files.size(); ++i)
  {
    retval << (dir.absolutePath() + "/" + files[i]);
  }
  QStringList folders = dir.entryList(QStringList(), QDir::AllDirs | QDir::NoDotAndDotDot);

  for (int i=0; i<folders.size(); ++i)
  {
    files = this->getAllFiles(folder + "/" + folders[i]);
    retval.append(files);
  }

  return retval;
}

void FileSelectWidget::updateComboBox()
{
  mDataComboBox->blockSignals(true);
  mDataComboBox->clear();

  QStringList files = this->getAllFiles(mRootPath);

  for (int i=0; i<files.size(); ++i)
  {
    mDataComboBox->addItem(QFileInfo(files[i]).fileName(), files[i]);
  }
  mDataComboBox->setCurrentIndex(-1);
  for (int i=0; i<mDataComboBox->count(); ++i)
  {
    if (mDataComboBox->itemData(i)==mFilename)
      mDataComboBox->setCurrentIndex(i);
  }

  mDataComboBox->setToolTip(mFilename);

  mDataComboBox->blockSignals(false);
}

void FileSelectWidget::currentDataComboIndexChanged(int index)
{
  if (index<0)
    return;

  mFilename = mDataComboBox->itemData(index).toString();
  emit fileSelected(mFilename);
}


}
