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

#include "sscVector3D.h"

namespace ssc
{

ImageTFData::ImageTFData() :
  mOpacityMapPtr(new IntIntMap()),
  mColorMapPtr(new ColorMap())
{
  mLevel = 1.0/2.0;
  mWindow = 1.0;
  mLLR = 0.0;
  mAlpha = 1.0;
}

ImageTFData::~ImageTFData()
{
}

void ImageTFData::initialize(double scalarMax)
{
  mLevel = scalarMax/2.0;
  mWindow = scalarMax;
  mLLR = 0.0;
  mAlpha = 1.0;
}
ImageTFDataPtr ImageTFData::createCopy()
{
  ImageTFDataPtr retval(new ImageTFData());
//  retval->mOpacityTF->DeepCopy(mOpacityTF);
//  retval->mColorTF->DeepCopy(mColorTF);
  retval->mOpacityMapPtr.reset(new IntIntMap(*mOpacityMapPtr));
  retval->mColorMapPtr.reset(new ColorMap(*mColorMapPtr));
//  if (mLut)
//  {
//    retval->mLut = vtkLookupTablePtr::New();
//    retval->mLut->DeepCopy(mLut);
//  }
  retval->mLevel = mLevel;
  retval->mWindow = mWindow;
  retval->mLLR = mLLR;
  retval->mAlpha = mAlpha;
  return retval;
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

//  std::cout << "void ImageTF3D::parseXml(QDomNode dataNode)" << std::endl;
}


/**Set Low Level Reject, meaning the lowest intensity
 * value that will be visible.
 */
void ImageTFData::setLLR(double val)
{
  if (similar(mLLR, val))
    return;

  mLLR = val;
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
void ImageTFData::addAlphaPoint( int alphaPosition , int alphaValue)
{
  mOpacityMapPtr->insert(std::pair<int, int>(alphaPosition, alphaValue));
  emit changed();
}
void ImageTFData::removeAlphaPoint(int alphaPosition)
{
  mOpacityMapPtr->erase(alphaPosition);
  emit changed();
}
void ImageTFData::setAlphaValue(int alphaPosition, int alphaValue)
{
  (*mOpacityMapPtr)[alphaPosition] = alphaValue;
  emit changed();
}
int ImageTFData::getAlphaValue(int alphaPosition)
{
  return (*mOpacityMapPtr)[alphaPosition];
}
void ImageTFData::addColorPoint( int colorPosition , QColor colorValue)
{
  mColorMapPtr->insert(std::pair<int, QColor>(colorPosition, colorValue));
  emit changed();
}
void ImageTFData::removeColorPoint(int colorPosition)
{
  mColorMapPtr->erase(colorPosition);
  emit changed();
}
void ImageTFData::setColorValue(int colorPosition, QColor colorValue)
{
  (*mColorMapPtr)[colorPosition] = colorValue;
  emit changed();
}

void ImageTFData::fillColorTFFromMap(vtkColorTransferFunctionPtr tf)
{
  // Create vtkColorTransferFunction from the color map
  tf->RemoveAllPoints();

  for (ColorMap::iterator iter = mColorMapPtr->begin(); iter != mColorMapPtr->end(); iter++)
    tf->AddRGBPoint(iter->first, iter->second.red()/255.0, iter->second.green()/255.0, iter->second.blue()/255.0);
}

void ImageTFData::fillOpacityTFFromMap(vtkPiecewiseFunctionPtr tf)
{
  // Create vtkPiecewiseFunction from the color map
  tf->RemoveAllPoints();
  for (IntIntMap::iterator iter = mOpacityMapPtr->begin(); iter != mOpacityMapPtr->end(); iter++)
    tf->AddPoint(iter->first, iter->second/255.0 );
}

}
