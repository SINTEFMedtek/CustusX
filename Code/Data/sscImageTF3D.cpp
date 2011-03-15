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
#include "sscMessageManager.h"
#include "vtkForwardDeclarations.h"

namespace ssc
{

//void ColorTF::addXml(QDomNode dataNode)
//{
//  QDomDocument doc = dataNode.ownerDocument();
//
//  QStringList pointStringList;
////  QDomElement colorNode = doc.createElement("color");
//  // Add color points
//  for (ColorMap::iterator colorPoint = mColorMapPtr->begin();
//       colorPoint != mColorMapPtr->end();
//       colorPoint++)
//    pointStringList.append(QString("%1=%2/%3/%4").arg(colorPoint->first).
//                           arg(colorPoint->second.red()).
//                           arg(colorPoint->second.green()).
//                           arg(colorPoint->second.blue()));
//  dataNode.appendChild(doc.createTextNode(pointStringList.join(" ")));
//}
//
//void ColorTF::parseXml(QDomNode dataNode)
//{
//  if (dataNode.isNull())
//    return;
//
//  mColorMapPtr->clear();
//  QStringList colorStringList = colorNode.toElement().text().split(" ");
//  for (int i = 0; i < colorStringList.size(); i++)
//  {
//    QStringList pointStringList = colorStringList[i].split("=");
//    QStringList valueStringList = pointStringList[1].split("/");
//    this->addColorPoint(pointStringList[0].toInt(),
//                  QColor(valueStringList[0].toInt(),
//                         valueStringList[1].toInt(),
//                         valueStringList[2].toInt()));
//  }
//}



///--------------------------------------------------------
///--------------------------------------------------------
///--------------------------------------------------------



ImageTF3D::ImageTF3D(vtkImageDataPtr base) :
	mOpacityTF(vtkPiecewiseFunctionPtr::New()),
	mColorTF(vtkColorTransferFunctionPtr::New()),	
	mBase(base)//,
//	mOpacityMapPtr(new IntIntMap()),
//	mColorMapPtr(new ColorMap())
{
  mUseTFMaps = true;
  mData.reset(new ImageTFData());
  connect(mData.get(), SIGNAL(changed()), this, SIGNAL(transferFunctionsChanged()));
  connect(mData.get(), SIGNAL(changed()), this, SLOT(transferFunctionsChangedSlot()));

	double max = getScalarMax();
  //messageManager()->sendDebug("For ImageTF3D image scalar range = " + string_cast(max));
	mLevel = max/2.0; 
	mWindow = max;
	mLLR = 0.0;
	mAlpha = 1.0;
	
	mColorTF->SetColorSpaceToRGB();
	
	mOpacityTF->AddPoint(0.0, 0.0);
	mOpacityTF->AddPoint(max, 1.0);
	
	mColorTF->AddRGBPoint(0.0, 0.0, 0.0, 0.0);
	mColorTF->AddRGBPoint(max, 1.0, 1.0, 1.0);

	mData->addAlphaPoint(this->getScalarMin(), 0);
	mData->addAlphaPoint(this->getScalarMax() * 0.1, 0);
	mData->addAlphaPoint(this->getScalarMax(), 255);
	mData->addColorPoint(this->getScalarMin(), Qt::black);
	mData->addColorPoint(this->getScalarMax(), Qt::white);
}

void ImageTF3D::transferFunctionsChangedSlot()
{
  this->refreshOpacityTF();
  this->refreshColorTF();
}

ImageTF3DPtr ImageTF3D::createCopy()
{
  ImageTF3DPtr retval(new ImageTF3D(mBase));
  retval->mOpacityTF->DeepCopy(mOpacityTF);
  retval->mColorTF->DeepCopy(mColorTF);
  retval->mUseTFMaps = mUseTFMaps;

  disconnect(mData.get(), SIGNAL(changed()), this, SIGNAL(transferFunctionsChanged()));
  disconnect(mData.get(), SIGNAL(changed()), this, SLOT(transferFunctionsChangedSlot()));
  retval->mData = mData->createCopy();
  connect(mData.get(), SIGNAL(changed()), this, SIGNAL(transferFunctionsChanged()));
  connect(mData.get(), SIGNAL(changed()), this, SLOT(transferFunctionsChangedSlot()));
//  retval->mOpacityMapPtr.reset(new IntIntMap(*mOpacityMapPtr));
//  retval->mColorMapPtr.reset(new ColorMap(*mColorMapPtr));
  if (mLut)
  {
    retval->mLut = vtkLookupTablePtr::New();
    retval->mLut->DeepCopy(mLut);
  }
//  retval->mLevel = mLevel;
//  retval->mWindow = mWindow;
//  retval->mLLR = mLLR;
//  retval->mAlpha = mAlpha;
  return retval;
}

void ImageTF3D::setVtkImageData(vtkImageDataPtr base)
{
	mBase = base;
}
void ImageTF3D::setOpacityTF(vtkPiecewiseFunctionPtr tf)
{
  mUseTFMaps = false;
	mOpacityTF = tf;
}

vtkPiecewiseFunctionPtr ImageTF3D::getOpacityTF()
{
	if (!mUseTFMaps) // sonowand hack .. while we figure out how to really do this....
	{
		refreshOpacityTF();
		return mOpacityTF;
	}
	
  OpacityMapPtr opacity = mData->getOpacityMap();

	// Create vtkPiecewiseFunction from the color map
	mOpacityTF->RemoveAllPoints();
	for (IntIntMap::iterator iter = opacity->begin(); iter != opacity->end(); iter++)
		mOpacityTF->AddPoint(iter->first, iter->second/255.0 );
	
	return mOpacityTF;
}

void ImageTF3D::setColorTF(vtkColorTransferFunctionPtr tf)
{
  mUseTFMaps = false;
	mColorTF = tf;
}

vtkColorTransferFunctionPtr ImageTF3D::getColorTF()
{
	//std::cout<<"ImageTF3D::getColorTF()" <<std::endl;
	
	if (!mUseTFMaps) // sonowand hack .. while we figure out how to really do this....
	{
		refreshColorTF();
		return mColorTF;
	}

  ColorMapPtr color = mData->getColorMap();

	// Create vtkColorTransferFunction from the color map
	mColorTF->RemoveAllPoints();
	
	for (ColorMap::iterator iter = color->begin(); iter != color->end(); iter++)
		mColorTF->AddRGBPoint(iter->first, iter->second.red()/255.0, 
													iter->second.green()/255.0, iter->second.blue()/255.0);

	return mColorTF;
}
	
void ImageTF3D::setLLR(double val)
{
  mData->setLLR(val);
}
double ImageTF3D::getLLR() const
{
	return mData->getLLR();
}
void ImageTF3D::setAlpha(double val)
{
  mData->setAlpha(val);
}
double ImageTF3D::getAlpha() const
{
	return mData->getAlpha();
}
void ImageTF3D::setWindow(double val)
{
  mData->setWindow(val);
}
double ImageTF3D::getWindow() const
{
	return mData->getWindow();
}
void ImageTF3D::setLevel(double val)
{
  mData->setLevel(val);
}

double ImageTF3D::getLevel() const
{
	return mData->getLevel();
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
  if (mUseTFMaps)
    return;

	if (!mLut)
	{
		return;
	}
	
	// Note on optimization:
	// the table generation can be moved to setLut(), leaving only
	// BuildFunctionFromTable here.
	
	double min = mData->getLevel() - ( mData->getWindow() / 2.0 );
	double max = mData->getLevel() + ( mData->getWindow() / 2.0 );
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
  if (mUseTFMaps)
    return;

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
	//double range = getScalarMax()-getScalarMin();
	double range = mData->getWindow();
	int smooth = (int)(0.1*range);
	mOpacityTF->AddPoint(mData->getLLR()-1, 0 );
	//mOpacityTF->AddPoint(mLLR-1*smooth, 0 );
	mOpacityTF->AddPoint(mData->getLLR(),          mAlpha*1/10 );
	mOpacityTF->AddPoint(mData->getLLR()+1*smooth, mAlpha*5/10 );
	mOpacityTF->AddPoint(mData->getLLR()+4*smooth, mAlpha );
#endif
	mOpacityTF->Update();
}

void ImageTF3D::addXml(QDomNode dataNode)
{
  QDomDocument doc = dataNode.ownerDocument();
//  QDomElement transferfunctionsNode = doc.createElement("transferfunctions");
//  parentNode.appendChild(transferfunctionsNode);

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

  dataNode.appendChild(alphaNode);
  dataNode.appendChild(colorNode);
}
void ImageTF3D::parseXml(QDomNode dataNode)
{
	if (dataNode.isNull())
		return;
	
	QDomNode alphaNode = dataNode.namedItem("alpha");
	// Read alpha node if it exists
	if (!alphaNode.isNull() && !alphaNode.toElement().text().isEmpty())
	{
    QString alphaString = alphaNode.toElement().text();
		mOpacityMapPtr->clear();
		QStringList alphaStringList = alphaString.split(" ");
		for (int i = 0; i < alphaStringList.size(); i++)
		{
			QStringList pointStringList = alphaStringList[i].split("=");
			if (pointStringList.size()<2)
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

//	std::cout << "void ImageTF3D::parseXml(QDomNode dataNode)" << std::endl;
}

}
