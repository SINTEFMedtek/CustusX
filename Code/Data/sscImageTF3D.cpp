/*
 * sscImageTF3D.cpp
 *
 *  Created on: Jan 9, 2009
 *      Author: christiana
 */

#include "sscImageTF3D.h"

#include <vector>
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
	//std::cout << "For ImageTF3D image scalar range = "<< max<<std::endl;
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
	//std::cout<<"ImageTF3D::getColorTF()" <<std::endl;
	
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
	val = std::max(1.0, val);

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
	if (lut==mLut)
		return;
	//std::cout<<"Lut set in 3d property" <<std::endl;
	mLut = lut;
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
	if (!mLut)
	{
		return;
	}
	
	// Note on optimization:
	// the table generation can be moved to setLut(), leaving only
	// BuildFunctionFromTable here.
	
	double min = mLevel - ( mWindow / 2.0 );
	double max = mLevel + ( mWindow / 2.0 );
	mColorTF->RemoveAllPoints();
	int N = mLut->GetNumberOfTableValues();
	//std::cout << " ImageTF3D::refreshColorTF(): range = [" << min << "," << max << "],\t lvl=" << mLevel << ",\t win=" << mWindow << std::endl;
	//std::cout << "mLut->GetNumberOfTableValues(): " << N  << std::endl;
	
	std::vector<double> function(N*3);
	for (int i=0; i<N; ++i)
	{
		double* color = mLut->GetTableValue(i);
		function[3*i+0] = color[0];
		function[3*i+1] = color[1];
		function[3*i+2] = color[2];
	}
	mColorTF->BuildFunctionFromTable(min, max, N, &*function.begin());
	
// equivalent way to go (slower):
	
//	// scale the lut from the |0,N> to |min,max> range,
//	// using index transformation i' = min + (max-min)/(N-1)*i
//	double delta = (max - min) / (N - 1);
//	
//	for (int i=0; i<N; ++i)
//	{
//		double* color = mLut->GetTableValue(i);
//		double index = min + delta*i;
//		mColorTF->AddRGBPoint(index, color[0], color[1], color[2]);
//	}
}

/**update the opacity TF according to the mLLR value
 */
void ImageTF3D::refreshOpacityTF()
{
	//std::cout << "ImageTF3D::refreshOpacityTF(): LLR=" << mLLR << ", Alpha=" << mAlpha << std::endl;

	mOpacityTF->RemoveAllPoints();
	//mOpacityTF->AddPoint(0.0, 0.0 );
#if 0 // old way - looks pretty but theoretically catastrophic
	mOpacityTF->AddPoint(mLLR, 0.0 );
	mOpacityTF->AddPoint(getScalarMax(), mAlpha );
#endif
#if 0 // correct way - step function - looks bad
	mOpacityTF->AddPoint(mLLR, 0.0 );
	mOpacityTF->AddPoint(mLLR+1, mAlpha );
#endif
#if 1 // middle way: cut at LLR, then a ramp up to alpha.
	double range = getScalarMax()-getScalarMin();
	int smooth = (int)(0.15*range);
	mOpacityTF->AddPoint(mLLR, 0 );
	//mOpacityTF->AddPoint(mLLR+1+smooth/2, mAlpha*1/4 );
	mOpacityTF->AddPoint(mLLR+1+1*smooth, mAlpha*2/4 );
	//mOpacityTF->AddPoint(mLLR+1+2*smooth, mAlpha*3/4 );
	mOpacityTF->AddPoint(mLLR+1+3*smooth, mAlpha );
	//mOpacityTF->AddPoint(getScalarMax(), mAlpha );
#endif
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
