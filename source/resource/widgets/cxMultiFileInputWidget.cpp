/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#include "cxMultiFileInputWidget.h"

#include <QtWidgets>

#include <iostream>

namespace cx
{


MultiFileInputWidget::MultiFileInputWidget(QWidget* parent) : QWidget(parent)
{
	mBasePath = "~";
	mUseRelativePath = false;
	mDescription = NULL;
	mBrowseButton = NULL;
	mFilenameEdit = NULL;

	mLayout = new QGridLayout(this);
	mLayout->setMargin(0);

	mFilenameEdit = new QTextEdit(this);
	mFilenameEdit->setToolTip("File names");
//	connect(mFilenameEdit, SIGNAL(editingFinished()), this, SIGNAL(fileChanged()));
//	connect(mFilenameEdit, SIGNAL(textChanged(const QString&)), this, SLOT(updateColor()));
	connect(mFilenameEdit, SIGNAL(textChanged()), this, SLOT(evaluateTextChanges()));
	mLayout->addWidget(mFilenameEdit, 0, 1);

	mBrowseAction = new QAction(QIcon(":/icons/open.png"), "Browse", this);
	mBrowseAction->setStatusTip("Browse the file system");
	connect(mBrowseAction, SIGNAL(triggered()), this, SLOT(browse()));

	mBrowseButton = new QToolButton();
	mBrowseButton->setDefaultAction(mBrowseAction);
	mLayout->addWidget(mBrowseButton, 0, 2);
}

void MultiFileInputWidget::setDescription(QString text)
{
	if (!mDescription)
	{
		mDescription = new QLabel(this);
		mLayout->addWidget(mDescription, 0,0);
	}

	mDescription->setText(text);
}

void MultiFileInputWidget::setFilenames(QStringList text)
{
	QStringList files = this->convertToAbsoluteFilenames(text);
	QStringList original = this->getAbsoluteFilenames();
//	QStringList original = this->convertToAbsoluteFilenames(mFilenameEdit->text().split('\n'));
	if (files==original)
		return;

	QString output = this->convertFilenamesToTextEditText(files);
	mFilenameEdit->setText(output);

//	if (mUseRelativePath)
//		text = QDir(mBasePath).relativeFilePath(text);
//
//	if (text==mFilenameEdit->text())
//		return;
//	mFilenameEdit->setText(text);
	this->widgetHasBeenChanged();
}

QStringList MultiFileInputWidget::convertToAbsoluteFilenames(QStringList text) const
{
	QStringList retval;
	for (unsigned i=0; i<text.size(); ++i)
		retval << this->convertToAbsoluteFilename(text[i]);
	return retval;
}
QString MultiFileInputWidget::convertToAbsoluteFilename(QString text) const
{
	if (text.isEmpty())
		return text;
	QString absolute = QDir(mBasePath).absoluteFilePath(text.trimmed());
	QString cleaned = QDir().cleanPath(absolute);
	if (!QFileInfo(cleaned).exists())
		return text; // return nonexistent files unchanged.
	return cleaned;
}
QString MultiFileInputWidget::convertFilenamesToTextEditText(QStringList files) const
{
	QStringList retval;
	for (unsigned i=0; i<files.size(); ++i)
	{
		retval << this->convertToPresentableFilename(files[i]);
	}
	return retval.join("\n");
}

QString MultiFileInputWidget::convertToPresentableFilename(QString absoluteFilename) const
{
	QString retval = absoluteFilename;
	if (!QFileInfo(absoluteFilename).exists())
		return retval;

	if (mUseRelativePath)
	{
		retval = QDir(mBasePath).relativeFilePath(absoluteFilename);
		if (retval.isEmpty())
			retval = "./"; // in order to separate from empty string (which appears during editing and should be ignored)
	}


	return retval;
}

void MultiFileInputWidget::widgetHasBeenChanged()
{
	this->updateHelpInternal();
	this->updateColor();
	emit fileChanged();
}

void MultiFileInputWidget::setHelp(QString text)
{
	mBaseHelp = text;
	this->updateHelpInternal();
}

void MultiFileInputWidget::updateHelpInternal()
{
	QString files = this->getAbsoluteFilenames().join("\n");
	QString text = QString("%1\n\n%2").arg(mBaseHelp).arg(files);
	mFilenameEdit->setToolTip(text);
	mFilenameEdit->setStatusTip(text);
}

void MultiFileInputWidget::setBrowseHelp(QString text)
{
	mBrowseAction->setToolTip(text);
	mBrowseAction->setStatusTip(text);
}

void MultiFileInputWidget::setBasePath(QString path)
{
	mBasePath = path;
	this->widgetHasBeenChanged();
}

void MultiFileInputWidget::browse()
{
	QString text = "Select file";
	if (mUseRelativePath)
		text = QString("Select file relative to %1").arg(mBasePath);

	QString filename = QFileDialog::getExistingDirectory(this, text, mBasePath);
	if (filename.isEmpty())
		return;
//	QString filename = QFileDialog::getOpenFileName(this, text, mBasePath);
//	if (filename.isEmpty())
//		return;

//	if (mUseRelativePath)
//		filename = QDir(mBasePath).relativeFilePath(filename);

	QStringList files = this->getAbsoluteFilenames();
	files << filename;
	this->setFilenames(files);
}

QStringList MultiFileInputWidget::getFilenames() const
{
	return mFilenameEdit->toPlainText().split('\n');
}

QStringList MultiFileInputWidget::getAbsoluteFilenames() const
{
	return this->convertToAbsoluteFilenames(mFilenameEdit->toPlainText().split("\n"));
//	QString absolute = QDir(mBasePath).absoluteFilePath(this->getFilename().trimmed());
//	QString cleaned = QDir().cleanPath(absolute);
//	return cleaned;
}

void MultiFileInputWidget::setUseRelativePath(bool on)
{
	mUseRelativePath = on;
	this->widgetHasBeenChanged();
}

void MultiFileInputWidget::evaluateTextChanges()
{
	this->updateHelpInternal();
	this->updateColor();
	QStringList current = this->getAbsoluteFilenames();
	if (current!=mLastFilenames)
	{
		mLastFilenames = current;
		emit fileChanged();
	}
}

void MultiFileInputWidget::updateColor()
{
	mFilenameEdit->blockSignals(true);
	QTextCursor cursor = mFilenameEdit->textCursor();
	int pos = cursor.position();

	QStringList org = mFilenameEdit->toPlainText().split("\n");
	mFilenameEdit->setPlainText("");
	for (int i=0; i<org.size(); ++i)
	{
		bool exists = QFileInfo(this->convertToAbsoluteFilename(org[i])).exists();

		if (exists)
			mFilenameEdit->setTextColor(QColor("black"));
		else
			mFilenameEdit->setTextColor(QColor("red"));
		if (i!=0)
			mFilenameEdit->textCursor().insertText("\n");
		mFilenameEdit->textCursor().insertText(org[i]);
	}

	cursor.setPosition(pos);

//	QTextBlockFormat red;
//	red.setProperty(QTextFormat::ForegroundBrush, QBrush(QColor("red")));
//	cursor.mergeBlockFormat(red);
	mFilenameEdit->setTextCursor(cursor);
	mFilenameEdit->blockSignals(false);

//	QColor color = QColor("black");
//	if (!QFileInfo(this->getAbsoluteFilename()).exists())
//		color = QColor("red");
//
//	QPalette p = mFilenameEdit->palette();
//	p.setColor(QPalette::Text, color);
//	mFilenameEdit->setPalette(p);
}

} /* namespace cx */
