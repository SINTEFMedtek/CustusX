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

namespace ssc
{

ImageTFData::ImageTFData(vtkImageDataPtr base) :
	mBase(base), mOpacityMapPtr(new IntIntMap()), mColorMapPtr(new ColorMap())
{
	double min = this->getScalarMin();
	double max = this->getScalarMax();

	// set a winlevel spanning the entire range
	mWindow = max - min;
	mLevel = min + mWindow / 2.0;
	// set llr/alpha with full transmission
	mLLR = min;
	mAlpha = 1.0;
}

ImageTFData::~ImageTFData()
{
}

void ImageTFData::setVtkImageData(vtkImageDataPtr base)
{
	mBase = base;
	emit changed();
}

vtkImageDataPtr ImageTFData::getVtkImageData()
{
	return mBase;
}

/**Return the maximum intensity value of the underlying dataset.
 * The range of all settings is |0,max>
 */
double ImageTFData::getScalarMax() const
{
	return mBase->GetScalarRange()[1];
}

double ImageTFData::getScalarMin() const
{
	return mBase->GetScalarRange()[0];
}

int ImageTFData::getMaxAlphaValue() const
{
	return 255;
}

/**set a lut that is used as a basis for the color tf.
 */
void ImageTFData::setLut(vtkLookupTablePtr lut)
{
	if (lut == mLut)
		return;
	mLut = lut;
	this->LUTChanged();
	//this->refreshColorTF();
}

vtkLookupTablePtr ImageTFData::getLut() const
{
	return mLut;
}

void ImageTFData::deepCopy(ImageTFData* source)
{
	mOpacityMapPtr.reset(new IntIntMap(*source->mOpacityMapPtr));
	mColorMapPtr.reset(new ColorMap(*source->mColorMapPtr));

	if (mLut)
	{
		mLut = vtkLookupTablePtr::New();
		mLut->DeepCopy(source->mLut);
	}
	mBase = source->mBase;

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
	for (IntIntMap::iterator opPoint = mOpacityMapPtr->begin(); opPoint != mOpacityMapPtr->end(); ++opPoint)
		pointStringList.append(QString("%1=%2").arg(opPoint->first). arg(opPoint->second));
	alphaNode.appendChild(doc.createTextNode(pointStringList.join(" ")));

	pointStringList.clear();
	QDomElement colorNode = doc.createElement("color");
	// Add color points
	for (ColorMap::iterator colorPoint = mColorMapPtr->begin(); colorPoint != mColorMapPtr->end(); ++colorPoint)
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
		mOpacityMapPtr->clear();
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
		mColorMapPtr->clear();
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

	//Repair TF's if faulty
//	this->fixTransferFunctions();

	mWindow = this->loadAttribute(dataNode, "window", mWindow);
	mLevel = this->loadAttribute(dataNode, "level", mLevel);
	mLLR = this->loadAttribute(dataNode, "llr", mLLR);
	mAlpha = this->loadAttribute(dataNode, "alpha", mAlpha);

	//  std::cout << "void ImageTF3D::parseXml(QDomNode dataNode)" << std::endl;
}

/**
 * Modify transfer function for unsigned CT.
 * This is necessary CT transfer functions are stored as signed.
 * \param onLoad true: add 1024 to preset transfer function values (Use after a preset is loaded)
 *               false: subtract 1024 from transfer function values (Use before saving a new preset, remember to revert back afterwards)
 */
void ImageTFData::unsignedCT(bool onLoad)
{
	//Signed after all. Don't do anyting
	if (this->getScalarMin() < 0)
		return;

	int modify = -1024;
	if(onLoad)
		modify = 1024;

	//	ssc::OpacityMapPtr opacityMap = this->getOpacityMap();
	OpacityMapPtr newOpacipyMap(new IntIntMap());
	for (ssc::IntIntMap::iterator it = this->getOpacityMap()->begin(); it != this->getOpacityMap()->end(); ++it)
		(*newOpacipyMap)[it->first + modify] = it->second;

	ColorMapPtr newColorMap(new ColorMap());
	for (ssc::ColorMap::iterator it = this->getColorMap()->begin(); it != this->getColorMap()->end(); ++it)
		(*newColorMap)[it->first + modify] = it->second;

	mOpacityMapPtr = newOpacipyMap;
	mColorMapPtr = newColorMap;

	mLevel = mLevel + modify;
	mLLR = mLLR + modify;
	// Don't emit for now. This function are used also for temporary modifications
	// Emit is moved to fixTransferFunctions()
//	emit changed();
}

void ImageTFData::fixTransferFunctions()
{
	//Make sure min and max values for transferfunctions are set

	ssc::OpacityMapPtr opacityMap = this->getOpacityMap();
	ssc::ColorMapPtr colorMap = this->getColorMap();

	vtkColorTransferFunctionPtr interpolatedTrFunc = vtkColorTransferFunctionPtr::New();
	this->fillColorTFFromMap(interpolatedTrFunc);

	if (opacityMap->find(this->getScalarMin()) == opacityMap->end())
	{
		this->addAlphaPoint(this->getScalarMin(), 0);
	}
	if (opacityMap->find(this->getScalarMax()) == opacityMap->end())
	{
		//The optimal solution may be to interpolate/extrapolate the max (min) values from the existing values
		//However, as most presets usually have all the top values set to white the error of the simpler code below is usually small
		ssc::IntIntMap::iterator opPoint = opacityMap->end();
		opPoint--;
		this->addAlphaPoint(this->getScalarMax(), opPoint->second);// Use value of current max element
	}
	if (colorMap->find(this->getScalarMin()) == colorMap->end())
	{
		this->addColorPoint(this->getScalarMin(), QColor(0, 0, 0));
	}
	if (colorMap->find(this->getScalarMax()) == colorMap->end())
	{
		//Interpolate to get correct color
		double* rgb = interpolatedTrFunc->GetColor(this->getScalarMax());
		QColor newColor = QColor(int(rgb[0] * 255), int(rgb[1] * 255), int(rgb[2] * 255));
		this->addColorPoint(this->getScalarMax(), newColor);
	}

	//Remove transfer function points outside range
	ssc::IntIntMap::iterator opIt = this->getOpacityMap()->begin();
	while (opIt != this->getOpacityMap()->end())
	{
		int delPoint = 1000000;
		if (opIt->first < this->getScalarMin())
			delPoint = opIt->first;
		else if (opIt->first > this->getScalarMax())
			delPoint = opIt->first;
		++opIt;

		if (delPoint != 1000000)
			this->removeAlphaPoint(delPoint);
	}

	ssc::ColorMap::iterator it = this->getColorMap()->begin();
	while (it != this->getColorMap()->end())
	{
		int delPoint = 1000000;
		if (it->first < this->getScalarMin())
			delPoint = it->first;
		else if (it->first > this->getScalarMax())
			delPoint = it->first;
		++it;

		if (delPoint != 1000000)
			this->removeColorPoint(delPoint);
	}
	emit changed();
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
	this->alphaLLRChanged();
	emit changed();
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
	this->alphaLLRChanged();
	emit changed();
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

	emit changed();
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
	emit changed();
}

double ImageTFData::getLevel() const
{
	return mLevel;
}

OpacityMapPtr ImageTFData::getOpacityMap()
{
	return mOpacityMapPtr;
}
ColorMapPtr ImageTFData::getColorMap()
{
	return mColorMapPtr;
}
void ImageTFData::addAlphaPoint(int alphaPosition, int alphaValue)
{
	//mOpacityMapPtr->insert(std::pair<int, int>(alphaPosition, alphaValue));
	(*mOpacityMapPtr)[alphaPosition] = alphaValue;
	emit changed();
}
void ImageTFData::removeAlphaPoint(int alphaPosition)
{
	mOpacityMapPtr->erase(alphaPosition);
	emit changed();
}
//void ImageTFData::setAlphaValue(int alphaPosition, int alphaValue)
//{
//  (*mOpacityMapPtr)[alphaPosition] = alphaValue;
//  emit changed();
//}
//int ImageTFData::getAlphaValue(int alphaPosition)
//{
//  return (*mOpacityMapPtr)[alphaPosition];
//}
void ImageTFData::addColorPoint(int colorPosition, QColor colorValue)
{
	(*mColorMapPtr)[colorPosition] = colorValue;
	//mColorMapPtr->insert(std::pair<int, QColor>(colorPosition, colorValue));
	this->colorMapChanged();
	emit changed();
}
void ImageTFData::removeColorPoint(int colorPosition)
{
	mColorMapPtr->erase(colorPosition);
	this->colorMapChanged();
	emit changed();
}
//void ImageTFData::setColorValue(int colorPosition, QColor colorValue)
//{
//  (*mColorMapPtr)[colorPosition] = colorValue;
//  this->colorMapChanged();
//  emit changed();
//}

void ImageTFData::fillColorTFFromMap(vtkColorTransferFunctionPtr tf)
{
	// Create vtkColorTransferFunction from the color map
	tf->RemoveAllPoints();

	for (ColorMap::iterator iter = mColorMapPtr->begin(); iter != mColorMapPtr->end(); ++iter)
		tf->AddRGBPoint(iter->first, iter->second.red() / 255.0, iter->second.green() / 255.0, iter->second.blue()
			/ 255.0);
	//tf->Update();
}

void ImageTFData::fillOpacityTFFromMap(vtkPiecewiseFunctionPtr tf)
{
	// Create vtkPiecewiseFunction from the color map
	tf->RemoveAllPoints();
	for (IntIntMap::iterator iter = mOpacityMapPtr->begin(); iter != mOpacityMapPtr->end(); ++iter)
		tf->AddPoint(iter->first, iter->second / 255.0);
	tf->Update();
}

double ImageTFData::mapThroughLUT(double x, int lutSize)
{
	double y = (mLLR - (mLevel - mWindow / 2)) / mWindow * lutSize;
	return y;
}

/**rebuild the output lut from all inputs.
 */
void ImageTFData::fillLUTFromLut(vtkLookupTablePtr output, vtkLookupTablePtr input)
{
	double b0 = mLevel - mWindow / 2.0;
	double b1 = mLevel + mWindow / 2.0;

	// find LLR on the lut:
	// We want to use the LLR on the _input_ intensity data, not on the
	// mapped data. Thus we map the llr through the lut and insert that value
	// into the lut.
	double llr = mapThroughLUT(mLLR, input->GetNumberOfTableValues());
	// if LLR < minimum table range, even the first value in the LUT will climb
	// above the llr. To avoid this, we use a hack that sets llr to at least 1.
	// This causes all zeros to become transparent, but the alternative is worse.
	// (what we really need is to subclass vtkLookupTable,
	//  but it contains nonvirtual functions).
	llr = std::max(1.0, llr); // hack.

	output->Build();
	output->SetNumberOfTableValues(input->GetNumberOfTableValues());
	output->SetTableRange(b0, b1);

	for (int i = 0; i < output->GetNumberOfTableValues(); ++i)
	{
		double rgba[4];
		input->GetTableValue(i, rgba);

		if (i >= llr)
			rgba[3] = 0.9999;
		else
			rgba[3] = 0.001;

		output->SetTableValue(i, rgba);
	}
	output->Modified();

	//  std::cout << "-----------------" << std::endl;
	//  for (unsigned i=0; i<256; i+=20)
	//  {
	//    testMap(i);
	//  }
	//  testMap(255);
	//  //mOutputLUT->Print(std::cout);
	//  //mOutputLUT->GetTable()->Print(std::cout);
	//  std::cout << "-----"
	//    << "llr=" << llr
	//    << ", LLR=" << mLLR
	//    << ", level=" << mLevel
	//    << ", window=" << mWindow
	//    << ", b=[" << b0 << "," << b1 << "] "
	//    << this
	//    << std::endl;
	//  std::cout << "-----------------" << std::endl;

}

void ImageTFData::colorMapChanged()
{
	this->buildLUTFromColorMap();
}

/**build the mBaseLUT from the mColorMap.
 * This overwrites the mBaseLUT.
 */
void ImageTFData::buildLUTFromColorMap()
{
	//  std::cout << "colormap begin" << std::endl;
	//  for (ColorMap::iterator iter=mColorMapPtr->begin(); iter!=mColorMapPtr->end(); ++iter)
	//  {
	//    std::cout << " " << iter->first << " " << iter->second.red() << " " << iter->second.green() << " " << iter->second.blue() << std::endl;
	//  }
	//  std::cout << "colormap end" << std::endl;

	int N = 0;
	if (!mColorMapPtr->empty())
		N = mColorMapPtr->rbegin()->first - mColorMapPtr->begin()->first; // largest key value in color map
	N += 1;//Because in the range 0 - 255 we have 256 values
	mLut = vtkLookupTablePtr::New(); // must reset in order to get the gpu buffering to reload
	mLut->Build();
	mLut->SetNumberOfTableValues(N);
	mLut->SetTableRange(0, N - 1);

	vtkColorTransferFunctionPtr colorFunc = vtkColorTransferFunctionPtr::New();
	this->fillColorTFFromMap(colorFunc);

	for (int i = 0; i < N; ++i)
	{
		double* rgb = colorFunc->GetColor(i + mColorMapPtr->begin()->first);
		mLut->SetTableValue(i, rgb[0], rgb[1], rgb[2], 1);
	}
	mLut->Modified();

	this->LUTChanged();
}

}
