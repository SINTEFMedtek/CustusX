/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/


/*
 * sscImageTFData.cpp
 *
 *  Created on: Mar 15, 2011
 *      Author: christiana
 */

#include "cxImageTFData.h"
#include <iostream>
#include <QDomDocument>
#include <QStringList>
#include <vtkColorTransferFunction.h>
#include <vtkPiecewiseFunction.h>
#include <vtkLookupTable.h>
#include <vtkImageData.h>

#include "cxVector3D.h"
#include "cxImageTF3D.h"

#include "cxTypeConversions.h"
#include "cxLogger.h"

namespace cx
{

ImageTFData::ImageTFData()
{
}

ImageTFData::~ImageTFData()
{
}

void ImageTFData::deepCopy(ImageTFData* source)
{
	mOpacityMap = source->mOpacityMap;
	mColorMap = source->mColorMap;
}

void ImageTFData::addXml(QDomNode dataNode)
{
	QDomDocument doc = dataNode.ownerDocument();

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
}

void ImageTFData::parseXml(QDomNode dataNode)
{
	if (dataNode.isNull())
	{
		CX_LOG_WARNING() << "ImageTFData::parseXml empty data node";
		return;
	}

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
		CX_LOG_WARNING() << "ImageTF3D::parseXml() found no alpha transferfunction";
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
		CX_LOG_WARNING() << "ImageTF3D::parseXml() found no color transferfunction";
	}

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
//	//Signed after all. Don't do anyting
//	if (this->getScalarMin() < 0)
//		return;

	int modify = -1024;
	if(onLoad)
		modify = 1024;

//	std::cout << "unsignedCT shift " << modify << std::endl;
	this->shift(modify);
}

void ImageTFData::shift(int val)
{
	this->shiftOpacity(val);
	this->shiftColor(val, 0, 1);

	this->internalsHaveChanged();
}

void ImageTFData::shiftColor(int shift, double center, double scale)
{
	ColorMap newColorMap;
	for (ColorMap::iterator it = mColorMap.begin(); it != mColorMap.end(); ++it)
	{
		double newVal = (it->first-center)*scale+center + shift;
		int roundedVal = floor(newVal + 0.5);
		newColorMap[roundedVal] = it->second;
	}
	mColorMap = newColorMap;
}

void ImageTFData::shiftOpacity(int shift)
{
	IntIntMap newOpacipyMap;
	for (IntIntMap::iterator it = mOpacityMap.begin(); it != mOpacityMap.end(); ++it)
	{
		newOpacipyMap[it->first + shift] = it->second;
	}
	mOpacityMap = newOpacipyMap;
}

/**Set Low Level Reject, meaning the lowest intensity
 * value that will be visible.
 */
void ImageTFData::setLLR(double val)
{
	double old = this->getLLR();
	if (similar(old, val))
		return;

	this->shiftOpacity(val-old);
	this->internalsHaveChanged();
}

double ImageTFData::getLLR() const
{
	if (mOpacityMap.empty())
		return 0;

	for (IntIntMap::const_iterator it = mOpacityMap.begin(); it != mOpacityMap.end(); ++it)
	{
		if (!similar(it->second, 0.0))
			return it->first;
	}
	return mOpacityMap.begin()->first;
}

void ImageTFData::setAlpha(double val)
{
	double old = this->getAlpha();
	if (similar(old, val))
		return;

	if (similar(old, 0.0))
	{
		// degenerate case: we have lost all info, simpl add input val to all but the first entry
		for (IntIntMap::iterator it = mOpacityMap.begin(); it != mOpacityMap.end(); ++it)
		{
			if (it==mOpacityMap.begin() && mOpacityMap.size()>1)
				continue; // heuristic: assume first entry should stay at zero
			it->second += val*255;
		}
	}
	else
	{
		double scale = val/old;
		for (IntIntMap::iterator it = mOpacityMap.begin(); it != mOpacityMap.end(); ++it)
		{
			it->second *= scale;
		}
	}

	this->internalsHaveChanged();
}

double ImageTFData::getAlpha() const
{
	double amax = 0;
	for (IntIntMap::const_iterator it = mOpacityMap.begin(); it != mOpacityMap.end(); ++it)
	{
		amax = std::max<double>(it->second, amax);
	}
	return amax/255;
}

/**Set Window, i.e. the size of the intensity
 * window that will be visible.
 */
void ImageTFData::setWindow(double val)
{
	double old = this->getWindow();
	val = std::max(1.0, val);

	if (similar(old, val))
		return;

	double scale = val/old;
	this->shiftColor(0, this->getLevel(), scale);

	this->internalsHaveChanged();
}

double ImageTFData::getWindow() const
{
	if (mColorMap.empty())
		return 0;
	return mColorMap.rbegin()->first - mColorMap.begin()->first;
//	return mWindow;
}

/**Set Level, i.e. the position of the intensity
 * window that will be visible.
 */
void ImageTFData::setLevel(double val)
{
	double old = this->getLevel();
	if (similar(old, val))
		return;
	double shift = val-old;

	this->shiftColor(shift, 0.0, 1.0);

//	mLevel = val;
	this->internalsHaveChanged();
}

double ImageTFData::getLevel() const
{
	if (mColorMap.empty())
		return 0;
	int a = mColorMap.begin()->first;
	int b = mColorMap.rbegin()->first;
	return a + (b-a)/2;
//	return mLevel;
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

vtkColorTransferFunctionPtr ImageTFData::generateColorTF() const
{
	vtkColorTransferFunctionPtr tf = vtkColorTransferFunctionPtr::New();
	this->fillColorTFFromMap(tf);
	return tf;
}

vtkPiecewiseFunctionPtr ImageTFData::generateOpacityTF() const
{
	vtkPiecewiseFunctionPtr tf = vtkPiecewiseFunctionPtr::New();
	this->fillOpacityTFFromMap(tf);
	return tf;
}

void ImageTFData::fillColorTFFromMap(vtkColorTransferFunctionPtr tf) const
{
	tf->SetColorSpaceToRGB();
	tf->RemoveAllPoints();
	for (ColorMap::const_iterator iter = mColorMap.begin(); iter != mColorMap.end(); ++iter)
	{
		QColor c = iter->second;
		tf->AddRGBPoint(iter->first, c.redF(), c.greenF(), c.blueF());
	}
}

void ImageTFData::fillOpacityTFFromMap(vtkPiecewiseFunctionPtr tf) const
{
	tf->RemoveAllPoints();
	for (IntIntMap::const_iterator iter = mOpacityMap.begin(); iter != mOpacityMap.end(); ++iter)
		tf->AddPoint(iter->first, iter->second / 255.0);
//	tf->Update();
}

}
