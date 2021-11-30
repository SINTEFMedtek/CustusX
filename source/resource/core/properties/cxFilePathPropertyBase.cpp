/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#include "cxFilePathPropertyBase.h"
#include <QDir>

namespace cx
{

void EmbeddedFilepath::appendRootPath(QString path)
{
	mRoots << path;
}

void EmbeddedFilepath::setFilepath(QString filename)
{
	mFilePath = filename;
}

void EmbeddedFilepath::evaluate(QString* foundRoot, bool* found, QString* foundRelative, QString* foundAbsolute) const
{
	*foundRelative = mFilePath;
	if (!mRoots.empty())
		*foundRoot = mRoots.front();
	*foundAbsolute = mFilePath;
	*found = false;

	foreach (QString root, mRoots)
	{
		root = QDir::cleanPath(root);
		if (!mFilePath.isEmpty() && QDir(root).exists(mFilePath))
		{
			*foundRelative = QDir(root).relativeFilePath(mFilePath);
			*foundRoot = root;
			*foundAbsolute = QDir(root).absoluteFilePath(mFilePath);
			*found = true;

			if (foundRelative->contains(".."))
			{
				// dont use relative paths outside of the roots
				*foundRelative = *foundAbsolute;
			}
			else
			{
				// if the current hit is inside the root, accept immediately
				return;
			}
		}
	}

}

QString EmbeddedFilepath::getRelativeFilepath() const
{
	bool found = false;
	QString root, relative, absolute;
	this->evaluate(&root, &found, &relative, &absolute);

	return relative;
}

QString EmbeddedFilepath::getAbsoluteFilepath() const
{
	bool found = false;
	QString root, relative, absolute;
	this->evaluate(&root, &found, &relative, &absolute);

	return absolute;
}

bool EmbeddedFilepath::exists() const
{
	bool found = false;
	QString root, relative, absolute;
	this->evaluate(&root, &found, &relative, &absolute);

	return found;
}

QString EmbeddedFilepath::getRootPath() const
{
	bool found = false;
	QString root, relative, absolute;
	this->evaluate(&root, &found, &relative, &absolute);

	return root;
}

QStringList EmbeddedFilepath::getRootPaths() const
{
	return mRoots;
}


FilePathPropertyBase::FilePathPropertyBase()
{

}

QString FilePathPropertyBase::getDisplayName() const
{
	return mName;
}

QString FilePathPropertyBase::getValue() const
{
	return mFilePath.getRelativeFilepath();
}

bool FilePathPropertyBase::setValue(const QString& val)
{
	if (val == this->getValue())
		return false;

	mFilePath.setFilepath(val);
	mStore.writeValue(mFilePath.getRelativeFilepath());
	emit valueWasSet();
	emit changed();
	return true;
}

EmbeddedFilepath FilePathPropertyBase::getEmbeddedPath()
{
	return mFilePath;
}

}//cx
