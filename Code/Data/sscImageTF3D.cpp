/*
 * sscImageTF3D.cpp
 *
 *  Created on: Jan 9, 2009
 *      Author: christiana
 */

#include "sscImageTF3D.h"

#include <vtkImageData.h>
#include <vtkWindowLevelLookupTable.h>
#include <vtkColorTransferFunction.h>
#include <vtkPiecewiseFunction.h>
#include <vtkVolumeProperty.h>
#include <QColor>
#include <QDomDocument>
#include <QStringList>
#include "sscVector3D.h"

typedef vtkSmartPointer<class vtkWindowLevelLookupTable> vtkWindowLevelLookupTablePtr;

namespace ssc
{

ImageTF3D::ImageTF3D(vtkImageDataPtr base) :
	mOpacityTF(vtkPiecewiseFunctionPtr::New()),
	mColorTF(vtkColorTransferFunctionPtr::New()),	
	mVolumeProperty(vtkVolumePropertyPtr::New()),
	mBase(base),
	mOpacityMapPtr(new IntIntMap()),
	mColorMapPtr(new ColorMap())
{
	double max = getScalarMax();
	std::cout << "For ImageTF3D image scalar range = "<< max<<std::endl;
	mLevel = max/2.0; 
	mWindow = max;
	mLLR = 0.0;
	mAlpha = 1.0;
	
	mColorTF->SetColorSpaceToRGB();
	
	mOpacityTF->AddPoint(0.0, 0.0);
	mOpacityTF->AddPoint(max, 1.0);
	
	mColorTF->AddRGBPoint(0.0, 0.0, 0.0, 0.0);
	mColorTF->AddRGBPoint(max, 1.0, 1.0, 1.0);
}
	
void ImageTF3D::setVtkImageData(vtkImageDataPtr base)
{
	mBase = base;
}
void ImageTF3D::setOpacityTF(vtkPiecewiseFunctionPtr tf)
{
	mOpacityTF = tf;
}

vtkPiecewiseFunctionPtr ImageTF3D::getOpacityTF()
{
	if (mLut) // sonowand hack .. while we figure out how to really do this.... 
	{
		refreshOpacityTF();
		return mOpacityTF;
	}
	
	// Create vtkPiecewiseFunction from the color map
	mOpacityTF->RemoveAllPoints();
	for (IntIntMap::iterator iter = mOpacityMapPtr->begin();
			 iter != mOpacityMapPtr->end(); iter++)
		mOpacityTF->AddPoint(iter->first, iter->second/255.0 );
	
	return mOpacityTF;
}

void ImageTF3D::setColorTF(vtkColorTransferFunctionPtr tf)
{
	mColorTF = tf;
}

vtkColorTransferFunctionPtr ImageTF3D::getColorTF()
{
	std::cout<<"ImageTF3D::getColorTF()" <<std::endl;
	
	if (mLut) // sonowand hack .. while we figure out how to really do this.... 
	{
		refreshColorTF();
		return mColorTF;
	}

	// Create vtkColorTransferFunction from the color map
	mColorTF->RemoveAllPoints();
	
	for (ColorMap::iterator iter = mColorMapPtr->begin();
			 iter != mColorMapPtr->end(); iter++)
		mColorTF->AddRGBPoint(iter->first, iter->second.red()/255.0, 
													iter->second.green()/255.0, iter->second.blue()/255.0);

	return mColorTF;
}
	
/**Set Low Level Reject, meaning the lowest intensity
 * value that will be visible.
 */
void ImageTF3D::setLLR(double val)
{
	if (similar(mLLR, val))
		return;

	mLLR = val;
	refreshOpacityTF();
}

double ImageTF3D::getLLR() const
{
	return mLLR;
}

void ImageTF3D::setAlpha(double val)
{
	if (similar(mAlpha, val))
		return;

	mAlpha = val;
	refreshOpacityTF();
}

double ImageTF3D::getAlpha() const
{
	return mAlpha;
}


/**Set Window, i.e. the size of the intensity
 * window that will be visible.
 */
void ImageTF3D::setWindow(double val)
{
	if (similar(mWindow, val))
		return;
	
	mWindow = val;
	//std::cout<<"setWindow, val: "<<val<<std::endl;
	refreshColorTF();
}

double ImageTF3D::getWindow() const
{
	return mWindow;
}

/**Set Level, i.e. the position of the intensity
 * window that will be visible.
 */
void ImageTF3D::setLevel(double val)
{
	if (similar(mLevel, val))
		return;
	
	//std::cout<<"setLevel, val: "<<val<<std::endl;
	mLevel = val;
	refreshColorTF();
}

double ImageTF3D::getLevel() const
{
	return mLevel;
}

/**set a lut that is used as a basis for the color tf.
 */
void ImageTF3D::setLut(vtkLookupTablePtr lut)
{
	std::cout<<"Lut set in 3d property" <<std::endl;
	mLut = lut;
	refreshColorTF();
}
void ImageTF3D::setTable(vtkUnsignedCharArrayPtr table)
{
	mLut->SetTable(table);
	refreshColorTF();
}
vtkLookupTablePtr ImageTF3D::getLut() const
{
	return mLut;
}

/**Return the maximum intensity value of the underlying dataset.
 * The range of all settings is |0,max>
 *
 */
double ImageTF3D::getScalarMax() const
{
	return 	mBase->GetScalarRange()[1];
}
	
double ImageTF3D::getScalarMin() const
{
	return 	mBase->GetScalarRange()[0];
}

/**update the color TF according to the
 * mLevel, mWindow and mLut values.
 */
void ImageTF3D::refreshColorTF()
{
	double min = mLevel - ( mWindow / 2.0 );
	double max = mLevel + ( mWindow / 2.0 );
	std::cout << " ImageTF3D::refreshColorTF() windowLevel = ["<<min<<","<<max<<"]"<<std::endl;
	//std::cout << " refresh colorTable windowLevel = ["<<min<<","<<max<<"]"<<std::endl;
	
	if (!mLut)
	{
		return;
	}

	mColorTF->RemoveAllPoints();
	mColorTF->AddRGBPoint(0.0, 0.0, 0.0, 0.0);
	mColorTF->AddRGBPoint(min, 0.0, 0.0, 0.0);
	mColorTF->AddRGBPoint(max, 1.0, 1.0, 1.0);
	mColorTF->AddRGBPoint(getScalarMax(), 1.0, 1.0, 1.0);
	
	int numColors = mLut->GetNumberOfTableValues();
	int step = numColors / 256;
	
	std::cout << "numColors " << numColors  << std::endl; 
	
	for (int i = 0; i < numColors; i += step)
	{
		double* color = mLut->GetTableValue(i);
		double index = min + double(i) * (max - min) / (numColors - 1);
		mColorTF->AddRGBPoint(index, color[0], color[1], color[2]);
		//std::cout << "color [" << i << "] " << Vector3D(color)  << std::endl; 
	}
}

/**update the opacity TF according to the mLLR value
 */
void ImageTF3D::refreshOpacityTF()
{
	//opacityFun->AddSegment( opacityLevel - 0.5*opacityWindow, 0.0,  opacityLevel + 0.5*opacityWindow, 1.0 );
	
	if (mLLR >= getScalarMax())
	{
		return;
	}

	mOpacityTF->RemoveAllPoints();
	mOpacityTF->AddPoint(0.0, 0.0 );
	mOpacityTF->AddPoint(mLLR, 0.0 );
	mOpacityTF->AddPoint(getScalarMax(), mAlpha );
	mOpacityTF->Update();
}

OpacityMapPtr ImageTF3D::getOpacityMap()
{
	return mOpacityMapPtr;
}
ColorMapPtr ImageTF3D::getColorMap()
{
	return mColorMapPtr;
}
void ImageTF3D::addAlphaPoint( int alphaPosition , int alphaValue)
{
	mOpacityMapPtr->insert(std::pair<int, int>(alphaPosition, alphaValue));
	emit transferFunctionsChanged();
}
void ImageTF3D::removeAlphaPoint(int alphaPosition)
{
	mOpacityMapPtr->erase(alphaPosition);
	emit transferFunctionsChanged();
}
void ImageTF3D::setAlphaValue(int alphaPosition, int alphaValue)
{
	(*mOpacityMapPtr)[alphaPosition] = alphaValue;
	emit transferFunctionsChanged();
}
int ImageTF3D::getAlphaValue(int alphaPosition)
{
	return (*mOpacityMapPtr)[alphaPosition];
}
void ImageTF3D::addColorPoint( int colorPosition , QColor colorValue)
{
	mColorMapPtr->insert(std::pair<int, QColor>(colorPosition, colorValue));
	emit transferFunctionsChanged();
}
void ImageTF3D::removeColorPoint(int colorPosition)
{
	mColorMapPtr->erase(colorPosition);
	emit transferFunctionsChanged();
}
void ImageTF3D::setColorValue(int colorPosition, QColor colorValue)
{
	(*mColorMapPtr)[colorPosition] = colorValue;
	emit transferFunctionsChanged();
}
QDomNode ImageTF3D::getXml(QDomDocument& doc)
{
	QDomElement transferfunctionsNode = doc.createElement("transferfunctions");
	
	QDomElement alphaNode = doc.createElement("alpha");
	// Use QStringList to put all points in the same string instead of storing 
	// the points as separate nodes.
	QStringList pointStringList;
	// Add alpha points
	for (IntIntMap::iterator opPoint = mOpacityMapPtr->begin();
       opPoint != mOpacityMapPtr->end();
       opPoint++)
		pointStringList.append(QString("%1=%2").arg(opPoint->first).
													 arg(opPoint->second));
	alphaNode.appendChild(doc.createTextNode(pointStringList.join(" ")));
	
	pointStringList.clear();
	QDomElement colorNode = doc.createElement("color");
	// Add color points
	for (ColorMap::iterator colorPoint = mColorMapPtr->begin();
       colorPoint != mColorMapPtr->end();
       colorPoint++)
		pointStringList.append(QString("%1=%2/%3/%4").arg(colorPoint->first).
													 arg(colorPoint->second.red()).
													 arg(colorPoint->second.green()).
													 arg(colorPoint->second.blue()));
	colorNode.appendChild(doc.createTextNode(pointStringList.join(" ")));
	
	transferfunctionsNode.appendChild(alphaNode);
	transferfunctionsNode.appendChild(colorNode);
	
	// Calling function must append this node
	return transferfunctionsNode;
}
void ImageTF3D::parseXml(QDomNode& dataNode)
{
	if (dataNode.isNull())
		return;
	
	QDomNode alphaNode = dataNode.namedItem("alpha");
	// Read alpha node if it exists
	if (!alphaNode.isNull())
	{
		mOpacityMapPtr->clear();
		QStringList alphaStringList = alphaNode.toElement().text().split(" ");
		for (int i = 0; i < alphaStringList.size(); i++)
		{
			QStringList pointStringList = alphaStringList[i].split("=");
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
	if (!colorNode.isNull())
	{
		mColorMapPtr->clear();
		QStringList colorStringList = colorNode.toElement().text().split(" ");
		for (int i = 0; i < colorStringList.size(); i++)
		{
			QStringList pointStringList = colorStringList[i].split("=");
			QStringList valueStringList = pointStringList[1].split("/");
			addColorPoint(pointStringList[0].toInt(), 
										QColor(valueStringList[0].toInt(),
													 valueStringList[1].toInt(),
													 valueStringList[2].toInt()));
		}
	}
	else
	{
		std::cout << "Warning: ImageTF3D::parseXml() found no color transferfunction";
		std::cout << std::endl;
	}
}
}
