// This file is part of CustusX, an Image Guided Therapy Application.
//
// Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
//
// CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
// code and binaries can only be used by SMT and those with explicit permission
// from SMT. CustusX shall not be distributed to anyone else.
//
// CustusX is a research tool. It is NOT intended for use or certified for use
// in a normal clinical setting. SMT does not take responsibility for its use
// in any way.
//
// See CustusX_License.txt for more information.

#include "cxFileInputWidget.h"
#include <QtGui>
#include "sscTypeConversions.h"
#include <iostream>

namespace cx
{

FileInputWidget::FileInputWidget(QWidget* parent) : QWidget(parent)
{
	mBasePath = "~";
	mUseRelativePath = false;
	mDescription = NULL;
	mBrowseButton = NULL;
	mFilenameEdit = NULL;

	mLayout = new QGridLayout(this);
	mLayout->setMargin(0);

	mFilenameEdit = new QLineEdit(this);
	mFilenameEdit->setToolTip("File name");
	connect(mFilenameEdit, SIGNAL(editingFinished()), this, SIGNAL(fileChanged()));
	connect(mFilenameEdit, SIGNAL(textChanged(const QString&)), this, SLOT(updateColor()));
	mLayout->addWidget(mFilenameEdit, 0, 1);

	mBrowseAction = new QAction(QIcon(":/icons/open.png"), "Browse", this);
	mBrowseAction->setStatusTip("Browse the file system");
	connect(mBrowseAction, SIGNAL(triggered()), this, SLOT(browse()));

	mBrowseButton = new QToolButton();
	mBrowseButton->setDefaultAction(mBrowseAction);
	mLayout->addWidget(mBrowseButton, 0, 2);
}

void FileInputWidget::setDescription(QString text)
{
	if (!mDescription)
	{
		mDescription = new QLabel(this);
		mLayout->addWidget(mDescription, 0,0);
	}

	mDescription->setText(text);
}

void FileInputWidget::setFilename(QString text)
{
	if (text==mFilenameEdit->text())
		return;
	mFilenameEdit->setText(text);
	this->widgetHasBeenChanged();
}

void FileInputWidget::widgetHasBeenChanged()
{
	this->updateHelpInternal();
	this->updateColor();
	emit fileChanged();
}

void FileInputWidget::setHelp(QString text)
{
	mBaseHelp = text;
	this->updateHelpInternal();
}

void FileInputWidget::updateHelpInternal()
{
	QString text = QString("%1\n%2").arg(this->getAbsoluteFilename()).arg(mBaseHelp);
	mFilenameEdit->setToolTip(text);
	mFilenameEdit->setStatusTip(text);
}

void FileInputWidget::setBrowseHelp(QString text)
{
	mBrowseAction->setToolTip(text);
	mBrowseAction->setStatusTip(text);
}

void FileInputWidget::setBasePath(QString path)
{
	mBasePath = path;
	this->widgetHasBeenChanged();
}

void FileInputWidget::browse()
{
	QString text = "Select file";
	if (mUseRelativePath)
		text = QString("Select file relative to %1").arg(mBasePath);

	QString filename = QFileDialog::getOpenFileName(this, text, mBasePath);
	if (filename.isEmpty())
		return;

	if (mUseRelativePath)
		filename = QDir(mBasePath).relativeFilePath(filename);

	this->setFilename(filename);
}

QString FileInputWidget::getFilename() const
{
	return mFilenameEdit->text();
}

QString FileInputWidget::getAbsoluteFilename() const
{
	QString absolute = QDir(mBasePath).absoluteFilePath(this->getFilename().trimmed());
	QString cleaned = QDir().cleanPath(absolute);
	return cleaned;
}

void FileInputWidget::setUseRelativePath(bool on)
{
	mUseRelativePath = on;
	this->widgetHasBeenChanged();
}

void FileInputWidget::updateColor()
{
	QColor color = QColor("black");
	if (!QFileInfo(this->getAbsoluteFilename()).exists())
		color = QColor("red");

	QPalette p = mFilenameEdit->palette();
	p.setColor(QPalette::Text, color);
	mFilenameEdit->setPalette(p);
}





} // namespace cx
