// This file is part of SSC,
// a C++ Library supporting Image Guided Therapy Applications.
//
// Copyright (C) 2008- SINTEF Medical Technology
// Copyright (C) 2008- Sonowand AS
//
// SSC is owned by SINTEF Medical Technology and Sonowand AS,
// hereafter named the owners. Each particular piece of code
// is owned by the part that added it to the library.
// SSC source code and binaries can only be used by the owners
// and those with explicit permission from the owners.
// SSC shall not be distributed to anyone else.
//
// SSC is distributed WITHOUT ANY WARRANTY; without even
// the implied warranty of MERCHANTABILITY or FITNESS FOR
// A PARTICULAR PURPOSE.
//
// See sscLicense.txt for more information.

/*
 * sscFileSelectWidget.cpp
 *
 *  Created on: May 6, 2011
 *      Author: christiana
 */

#include <sscFileSelectWidget.h>
#include <QtGui>
#include "sscTypeConversions.h"
#include <iostream>

namespace ssc
{

FileSelectWidget::FileSelectWidget(QWidget* parent)
{
	mNameFilters << "*.mhd";

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

QString FileSelectWidget::getFilename() const
{
	return mFilename;
}

void FileSelectWidget::setFilename(QString name)
{
	mFilename = name;
	//  QString temp = QFileInfo(mFilename).dir().absolutePath();
	//  std::cout << "isdir " << QFileInfo(mFilename).isDir() << std::endl;
	//  std::cout << "FileSelectWidget::setFilename name" << mFilename << '\n' << QFileInfo(mFilename).dir().absolutePath() << std::endl;

	if (QFileInfo(mFilename).isDir())
	{
		mRootPath = QFileInfo(mFilename).dir().absolutePath();
		//    std::cout << "FileSelectWidget::setFilename root" << mRootPath << std::endl;
	}
	else
	{
		if (mRootPath.isEmpty())
			mRootPath = QFileInfo(mFilename).dir().absolutePath();
	}



	this->refresh();
}

void FileSelectWidget::setNameFilter(QStringList filter)
{
	mNameFilters = filter;
}

void FileSelectWidget::setPath(QString path)
{
	mRootPath = path;
	//  std::cout << "FileSelectWidget::setPath root" << mRootPath << std::endl;
	this->refresh();
}

void FileSelectWidget::selectData()
{
	QString filter = mNameFilters.join(";;");
	QString filename = QFileDialog::getOpenFileName(this, QString(tr("Select file")), mRootPath, filter);

	if (filename.isEmpty())
		return;

	mFilename = filename;
	//  std::cout << this << " selectData " << mFilename << std::endl;

	this->refresh();
	emit fileSelected(mFilename);
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

	QStringList files = this->getAllFiles(mRootPath, 3);

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
