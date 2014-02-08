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
 * sscImageTFData.cpp
 *
 *  Created on: Mar 15, 2011
 *      Author: christiana
 */

#include <sscImageTFData.h>
#include <iostream>
#include <QDomDocument>
#include <QStringList>
#include <vtkColorTransferFunction.h>
#include <vtkPiecewiseFunction.h>
#include <vtkLookupTable.h>
#include <vtkImageData.h>

#include "sscVector3D.h"
#include "sscImageTF3D.h"
#include "sscLogger.h"
#include "sscTypeConversions.h"

namespace cx
{

ImageTFData::ImageTFData()
{
}

ImageTFData::~ImageTFData()
{
}

/**set a lut that is used as a basis for the color tf.
 */
void ImageTFData::setLut(vtkLookupTablePtr lut)
{
	std::cout << "ImageTFData::setLut not used" << std::endl;
//	if (lut == mLut)
//		return;
//	mLut = lut;
//	this->LUTChanged();
//	//this->refreshColorTF();
}

void ImageTFData::deepCopy(ImageTFData* source)
{
	mOpacityMap = source->mOpacityMap;
	mColorMap = source->mColorMap;

	mLevel = source->mLevel;
	mWindow = source->mWindow;
	mLLR = source->mLLR;
	mAlpha = source->mAlpha;
}

void ImageTFData::addXml(QDomNode dataNode)
{
	QDomDocument doc = dataNode.ownerDocument();
	//  QDomElement transferfunctionsNode = doc.createElement("transferfunctions");
	//  parentNode.appendChild(transferfunctionsNode);

	QDomElement alphaNode = doc.createElement("alpha");
	// Use QStringList to put all points in the same string instead of storing
	// the points as separate nodes.
	QStringList pointStringList;
	// Add alpha points
	for (IntIntMap::iterator opPoint = mOpacityMap.begin(); opPoint != mOpacityMap.end(); ++opPoint)
		pointStringList.append(QString("%1=%2").arg(opPoint->first). arg(opPoint->second));
	alphaNode.appendChild(doc.createTextNode(pointStringList.join(" ")));

	pointStringList.clear();
	QDomElement colorNode = doc.createElement("color");
	// Add color points
	for (ColorMap::iterator colorPoint = mColorMap.begin(); colorPoint != mColorMap.end(); ++colorPoint)
		pointStringList.append(QString("%1=%2/%3/%4").arg(colorPoint->first). arg(colorPoint->second.red()). arg(
			colorPoint->second.green()). arg(colorPoint->second.blue()));
	colorNode.appendChild(doc.createTextNode(pointStringList.join(" ")));

	dataNode.appendChild(alphaNode);
	dataNode.appendChild(colorNode);

	QDomElement elem = dataNode.toElement();
	elem.setAttribute("window", mWindow);
	elem.setAttribute("level", mLevel);
	elem.setAttribute("llr", mLLR);
	elem.setAttribute("alpha", mAlpha);
}

void ImageTFData::parseXml(QDomNode dataNode)
{
	if (dataNode.isNull())
		return;

	QDomNode alphaNode = dataNode.namedItem("alpha");
	// Read alpha node if it exists
	if (!alphaNode.isNull() && !alphaNode.toElement().text().isEmpty())
	{
		QString alphaString = alphaNode.toElement().text();
		mOpacityMap.clear();
		QStringList alphaStringList = alphaString.split(" ", QString::SkipEmptyParts);
		for (int i = 0; i < alphaStringList.size(); i++)
		{
			QStringList pointStringList = alphaStringList[i].split("=");
			if (pointStringList.size() < 2)
				continue;
			addAlphaPoint(pointStringList[0].toInt(), pointStringList[1].toInt());
		}
	}
	else
	{
		std::cout << "Warning: ImageTF3D::parseXml() found no alpha transferfunction";
		std::cout << std::endl;
	}

	QDomNode colorNode = dataNode.namedItem("color");
	// Read color node if it exists
	if (!colorNode.isNull() && !colorNode.toElement().text().isEmpty())
	{
		mColorMap.clear();
		QStringList colorStringList = colorNode.toElement().text().split(" ", QString::SkipEmptyParts);
		for (int i = 0; i < colorStringList.size(); i++)
		{
			QStringList pointStringList = colorStringList[i].split("=");
			QStringList valueStringList = pointStringList[1].split("/");
			addColorPoint(pointStringList[0].toInt(), QColor(valueStringList[0].toInt(), valueStringList[1].toInt(),
				valueStringList[2].toInt()));
		}
	}
	else
	{
		std::cout << "Warning: ImageTF3D::parseXml() found no color transferfunction";
		std::cout << std::endl;
	}

//	std::cout << "window " << mWindow << std::endl;
	mWindow = this->loadAttribute(dataNode, "window", mWindow);
	mLevel = this->loadAttribute(dataNode, "level", mLevel);
	mLLR = this->loadAttribute(dataNode, "llr", mLLR);
	mAlpha = this->loadAttribute(dataNode, "alpha", mAlpha);
//	std::cout << "window " << mWindow << std::endl;

	//  std::cout << "void ImageTF3D::parseXml(QDomNode dataNode)" << std::endl;

	this->internalsHaveChanged();
}

/**
 * Modify transfer function for unsigned CT.
 * This is necessary CT transfer functions are stored as signed.
 * \param onLoad true: add 1024 to preset transfer function values (Use after a preset is loaded)
 *               false: subtract 1024 from transfer function values (Use before saving a new preset, remember to revert back afterwards)
 */
void ImageTFData::unsignedCT(bool onLoad)
{
	// REMOVED CA 2014-02-07 - TODO

//	//Signed after all. Don't do anyting
//	if (this->getScalarMin() < 0)
//		return;

	int modify = -1024;
	if(onLoad)
		modify = 1024;

//	modify=0;
	std::cout << "unsignedCT shift " << modify << std::endl;
	this->shift(modify);
}

void ImageTFData::shift(int val)
{
	int modify = val;

	//	OpacityMapPtr opacityMap = this->getOpacityMap();
	IntIntMap newOpacipyMap;
	for (IntIntMap::iterator it = mOpacityMap.begin(); it != mOpacityMap.end(); ++it)
		newOpacipyMap[it->first + modify] = it->second;

	ColorMap newColorMap;
	for (ColorMap::iterator it = mColorMap.begin(); it != mColorMap.end(); ++it)
		newColorMap[it->first + modify] = it->second;

	mOpacityMap = newOpacipyMap;
	mColorMap = newColorMap;

	//mLevel = mLevel + modify;
	mLLR = mLLR + modify;

	this->internalsHaveChanged();
}

double ImageTFData::loadAttribute(QDomNode dataNode, QString name, double defVal)
{
	QString text = dataNode.toElement().attribute(name);
	bool ok;
	double val = text.toDouble(&ok);
	if (ok)
		return val;
	return defVal;
}

/**Set Low Level Reject, meaning the lowest intensity
 * value that will be visible.
 */
void ImageTFData::setLLR(double val)
{
	if (similar(mLLR, val))
		return;

	mLLR = val;
	this->internalsHaveChanged();
}

double ImageTFData::getLLR() const
{
	return mLLR;
}

void ImageTFData::setAlpha(double val)
{
	if (similar(mAlpha, val))
		return;

	mAlpha = val;
	this->internalsHaveChanged();
}

double ImageTFData::getAlpha() const
{
	return mAlpha;
}

/**Set Window, i.e. the size of the intensity
 * window that will be visible.
 */
void ImageTFData::setWindow(double val)
{
	val = std::max(1.0, val);

	if (similar(mWindow, val))
		return;

	mWindow = val;

	this->internalsHaveChanged();
}

double ImageTFData::getWindow() const
{
	return mWindow;
}

/**Set Level, i.e. the position of the intensity
 * window that will be visible.
 */
void ImageTFData::setLevel(double val)
{
	if (similar(mLevel, val))
		return;

	mLevel = val;
	this->internalsHaveChanged();
}

double ImageTFData::getLevel() const
{
	return mLevel;
}

IntIntMap ImageTFData::getOpacityMap()
{
	return mOpacityMap;
}
ColorMap ImageTFData::getColorMap()
{
	return mColorMap;
}
void ImageTFData::addAlphaPoint(int alphaPosition, int alphaValue)
{
	//mOpacityMapPtr->insert(std::pair<int, int>(alphaPosition, alphaValue));
	mOpacityMap[alphaPosition] = alphaValue;
	this->internalsHaveChanged();
}
void ImageTFData::removeAlphaPoint(int alphaPosition)
{
	mOpacityMap.erase(alphaPosition);
	this->internalsHaveChanged();
}
void ImageTFData::moveAlphaPoint(int oldpos, int newpos, int alphaValue)
{
	mOpacityMap.erase(oldpos);
	mOpacityMap[newpos] = alphaValue;
	this->internalsHaveChanged();
}
void ImageTFData::addColorPoint(int colorPosition, QColor colorValue)
{
	mColorMap[colorPosition] = colorValue;
	//mColorMapPtr->insert(std::pair<int, QColor>(colorPosition, colorValue));
	this->internalsHaveChanged();
}
void ImageTFData::removeColorPoint(int colorPosition)
{
	mColorMap.erase(colorPosition);
	this->internalsHaveChanged();
}

void ImageTFData::moveColorPoint(int oldpos, int newpos, QColor colorValue)
{
	mColorMap.erase(oldpos);
	mColorMap[newpos] = colorValue;
	this->internalsHaveChanged();
}

void ImageTFData::resetAlpha(IntIntMap val)
{
	mOpacityMap = val;
	this->internalsHaveChanged();
}

void ImageTFData::resetColor(ColorMap val)
{
	mColorMap = val;
	this->internalsHaveChanged();
}

void ImageTFData::fillColorTFFromMap(vtkColorTransferFunctionPtr tf)
{
	tf->SetColorSpaceToRGB();
	tf->RemoveAllPoints();
	for (ColorMap::iterator iter = mColorMap.begin(); iter != mColorMap.end(); ++iter)
	{
		QColor c = iter->second;
		tf->AddRGBPoint(iter->first, c.redF(), c.greenF(), c.blueF());
	}
}

void ImageTFData::fillOpacityTFFromMap(vtkPiecewiseFunctionPtr tf)
{
	tf->RemoveAllPoints();
	for (IntIntMap::iterator iter = mOpacityMap.begin(); iter != mOpacityMap.end(); ++iter)
		tf->AddPoint(iter->first, iter->second / 255.0);
	tf->Update();
}

}
