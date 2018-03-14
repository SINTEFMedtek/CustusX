/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/


/*
 * sscFileSelectWidget.cpp
 *
 *  Created on: May 6, 2011
 *      Author: christiana
 */

#include "cxFileSelectWidget.h"
#include <QtWidgets>

#include "cxTypeConversions.h"
#include <iostream>
#include <QHBoxLayout>
#include <QtWidgets>

namespace cx
{

FileSelectWidget::FileSelectWidget(QWidget* parent)
{
	mNameFilters << "*.mhd";
	mFolderDepth = 2;

	QHBoxLayout* dataLayout = new QHBoxLayout(this);
	dataLayout->setMargin(0);
	mDataComboBox = new QComboBox(this);
	connect(mDataComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(currentDataComboIndexChanged(int)));

	mSelectDataAction = new QAction(QIcon(":/icons/open.png"), tr("&Select data"), this);
	connect(mSelectDataAction, SIGNAL(triggered()), this, SLOT(selectData()));
	mSelectDataButton = new QToolButton(this);
	mSelectDataButton->setDefaultAction(mSelectDataAction);

	dataLayout->addWidget(mDataComboBox);
	dataLayout->addWidget(mSelectDataButton);
}

void FileSelectWidget::setFolderDepth(int depth)
{
	mFolderDepth = depth;
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
		mRootPaths.clear();
		mRootPaths << QFileInfo(mFilename).dir().absolutePath();
	}
	else
	{
		if (mRootPaths.isEmpty())
			mRootPaths << QFileInfo(mFilename).dir().absolutePath();
	}



	this->refresh();
}

void FileSelectWidget::setNameFilter(QStringList filter)
{
	mNameFilters = filter;
}

void FileSelectWidget::setPath(QString path)
{
	this->setPaths(QStringList() << path);
//	mRootPath = path;
//	//  std::cout << "FileSelectWidget::setPath root" << mRootPath << std::endl;
//	this->refresh();
}

void FileSelectWidget::setPaths(QStringList paths)
{
	mRootPaths = paths;
	this->refresh();
}

void FileSelectWidget::selectData()
{
	QString filter = mNameFilters.join(";;");
	QString folder;
	if (!mRootPaths.isEmpty())
		folder = mRootPaths.front();
	QString filename = QFileDialog::getOpenFileName(this, QString(tr("Select file")), folder, filter);

	if (filename.isEmpty())
		return;

	mFilename = filename;
	//  std::cout << this << " selectData " << mFilename << std::endl;

	this->refresh();
	emit fileSelected(mFilename);
}

QStringList FileSelectWidget::getAllFiles()
{
    QStringList files;
    for (int i=0; i<mRootPaths.size(); ++i)
        files << this->getAllFiles(mRootPaths[i], mFolderDepth);
    return files;
}


QStringList FileSelectWidget::getAllFiles(QString folder, int depth)
{
	QDir dir(folder);
	QStringList files = dir.entryList(mNameFilters, QDir::Files);

	QStringList retval;
	for (int i = 0; i < files.size(); ++i)
	{
		retval << (dir.absolutePath() + "/" + files[i]);
	}
	QStringList folders = dir.entryList(QStringList(), QDir::AllDirs | QDir::NoDotAndDotDot);

	if (depth>0)
	{
		for (int i = 0; i < folders.size(); ++i)
		{
			files = this->getAllFiles(folder + "/" + folders[i], depth-1);
			retval.append(files);
		}
	}

	return retval;
}

void FileSelectWidget::refresh()
{
	this->updateComboBox();
}

void FileSelectWidget::updateComboBox()
{
	mDataComboBox->blockSignals(true);
	mDataComboBox->clear();

	QStringList files;
	for (int i=0; i<mRootPaths.size(); ++i)
		files << this->getAllFiles(mRootPaths[i], mFolderDepth);

	for (int i = 0; i < files.size(); ++i)
	{
		mDataComboBox->addItem(QFileInfo(files[i]).fileName(), files[i]);
	}
	mDataComboBox->setCurrentIndex(-1);
	for (int i = 0; i < mDataComboBox->count(); ++i)
	{
		if (mDataComboBox->itemData(i) == mFilename)
			mDataComboBox->setCurrentIndex(i);
	}

	if (!mFilename.isEmpty() && mDataComboBox->currentIndex() < 0 && !files.contains(mFilename))
	{
		mDataComboBox->addItem(QFileInfo(mFilename).fileName(), mFilename);
		mDataComboBox->setCurrentIndex(mDataComboBox->count() - 1);
	}

	mDataComboBox->setToolTip(mFilename);

	mDataComboBox->blockSignals(false);
}

void FileSelectWidget::currentDataComboIndexChanged(int index)
{
	if (index < 0)
		return;

	mFilename = mDataComboBox->itemData(index).toString();
	emit fileSelected(mFilename);
}

}
