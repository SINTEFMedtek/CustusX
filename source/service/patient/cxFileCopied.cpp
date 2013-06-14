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

#include "cxFileCopied.h"

#include <QtCore>
#include "sscData.h"
#include "sscMessageManager.h"

namespace cx
{

FileCopied::FileCopied(const QString& absolutefilePath, const QString& relativefilePath, ssc::DataPtr data) :
				mFilePath(absolutefilePath), mRelativeFilePath(relativefilePath), mData(data)
{
}

void FileCopied::areFileCopiedSlot()
{
	//messageMan()->sendInfo("Check if file is copied: "+mFilePath);

	QFile file(mFilePath);

	bool correctCopy = false;
	QFileInfo fileInfo(mFilePath);
	QString fileType = fileInfo.suffix();

	if (!file.exists() || !file.open(QIODevice::NotOpen | QIODevice::ReadOnly))
	{
		ssc::messageManager()->sendWarning("File is not copied: " + mFilePath + " Cannot open file.");
		file.close();
		QTimer::singleShot(5000, this, SLOT(areFileCopiedSlot())); // Wait another 5 seconds
	}
	else if (fileType.compare("mhd", Qt::CaseInsensitive) == 0 || fileType.compare("mha", Qt::CaseInsensitive) == 0)
	{
		bool foundDimSize = false;
		bool foundElementType = false;
		// Parse file to check if copied correctly
		QTextStream stream(&file);
		QString sLine;
		QRegExp rx("([\\d\\.]+)\\s+([\\d\\.]+)\\s+([\\d\\.]+)");
		QRegExp rxDimSize("DimSize =");
		QRegExp rxElementType("ElementType = ");
		int numElements = 0;
		bool end = false;
		int elementSize = 1;

		while (!end && !stream.atEnd())
		{
			sLine = stream.readLine(); // line of text excluding '\n'
			if (sLine.isEmpty())
				end = true;
			//messageMan()->sendInfo("line: "+sLine.toStdString());
			{
				rxDimSize.indexIn(sLine);
				if (rxDimSize.pos() != -1)
				{
					rx.indexIn(sLine);
					rx.pos();
					QStringList list = rx.capturedTexts();
					numElements = list[1].toInt() * list[2].toInt() * list[3].toInt();
					foundDimSize = true;
				}
				rxElementType.indexIn(sLine);
				if (rxElementType.pos() != -1)
				{
					rx.indexIn(sLine);
					rx.pos();
					QString elementType = rx.cap();
					ssc::messageManager()->sendInfo("ElementType: " + elementType);
					if (elementType == "MET_USHORT") //16 bit
						elementSize = 2;
					else if (elementType == "MET_SHORT")
						elementSize = 2;
					else
						// 8 bit
						elementSize = 1;
					foundElementType = true;
				}
				if (foundDimSize && foundElementType)
					end = true;
			}
		}
		if (!file.flush())
			ssc::messageManager()->sendWarning("Flush error");
		file.close();

		if (!foundDimSize)
		{
			ssc::messageManager()->sendWarning("File is not copied correctly: " + mFilePath + " Parts missing");
		}
		else
		{
			QRegExp rxFileype;
			QString rawFilepath = mFilePath;
			if (rawFilepath.endsWith(".mhd"))
				rxFileype.setPattern("\\.mhd");
			else if (rawFilepath.endsWith(".mha"))
				rxFileype.setPattern("\\.mha");
			rawFilepath = rawFilepath.replace(rxFileype, ".raw");
			QFile rawFile(rawFilepath);
			rawFile.open(QIODevice::ReadOnly);

			//Test if raw file is large enough
			if (rawFile.bytesAvailable() < (numElements * elementSize))
			{
				ssc::messageManager()->sendWarning("File is not copied correctly: " + rawFilepath + " Parts missing");
			}
			else
				correctCopy = true;

		}
	}
	else if (fileType.compare("stl", Qt::CaseInsensitive) == 0)
	{
		//TODO: Check intergity of file
		correctCopy = true;
	}
	else if (fileType.compare("vtk", Qt::CaseInsensitive) == 0)
	{
		//TODO: Check intergity of file
		correctCopy = true;
	}

	if (!correctCopy)
	{
		ssc::messageManager()->sendWarning("File(s) not copied correctly - wait another 5 seconds");
		QTimer::singleShot(5000, this, SLOT(areFileCopiedSlot()));
	}
	else
	{
		ssc::messageManager()->sendInfo("File copied correctly: " + mFilePath);
		mData->setFilePath(mRelativeFilePath); // Update file path

		//Save patient, to avoid problems
		emit fileCopiedCorrectly();
	}
}

} // namespace cx
