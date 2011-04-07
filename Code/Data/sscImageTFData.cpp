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

namespace ssc
{

ImageTFData::ImageTFData(vtkImageDataPtr base) :
      mBase(base), mOpacityMapPtr(new IntIntMap()), mColorMapPtr(new ColorMap())
{
  double min = this->getScalarMin();
  double max = this->getScalarMax();

  // set a winlevel spanning the entire range
  mWindow = max-min;
  mLevel = min + mWindow/2.0;
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
}

/**Return the maximum intensity value of the underlying dataset.
 * The range of all settings is |0,max>
 */
double ImageTFData::getScalarMax() const
{
  return  mBase->GetScalarRange()[1];
}

double ImageTFData::getScalarMin() const
{
  return  mBase->GetScalarRange()[0];
}

int ImageTFData::getMaxAlphaValue() const
{
  return 255;
}

/**set a lut that is used as a basis for the color tf.
 */
void ImageTFData::setLut(vtkLookupTablePtr lut)
{
  if (lut==mLut)
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
    QStringList colorStringList = colorNode.toElement().text().split(" ", QString::SkipEmptyParts);
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

  mWindow = this->loadAttribute(dataNode, "window", mWindow);
  mLevel = this->loadAttribute(dataNode, "level", mLevel);
  mLLR = this->loadAttribute(dataNode, "llr", mLLR);
  mAlpha = this->loadAttribute(dataNode, "alpha", mAlpha);

//  std::cout << "void ImageTF3D::parseXml(QDomNode dataNode)" << std::endl;
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
void ImageTFData::addAlphaPoint( int alphaPosition , int alphaValue)
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
void ImageTFData::addColorPoint( int colorPosition , QColor colorValue)
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

  for (ColorMap::iterator iter = mColorMapPtr->begin(); iter != mColorMapPtr->end(); iter++)
    tf->AddRGBPoint(iter->first, iter->second.red()/255.0, iter->second.green()/255.0, iter->second.blue()/255.0);
  //tf->Update();
}

void ImageTFData::fillOpacityTFFromMap(vtkPiecewiseFunctionPtr tf)
{
  // Create vtkPiecewiseFunction from the color map
  tf->RemoveAllPoints();
  for (IntIntMap::iterator iter = mOpacityMapPtr->begin(); iter != mOpacityMapPtr->end(); iter++)
    tf->AddPoint(iter->first, iter->second/255.0 );
  tf->Update();
}

double ImageTFData::mapThroughLUT(double x, int lutSize)
{
  double y = (mLLR - (mLevel - mWindow/2))/mWindow * lutSize;
  return y;
}


/**rebuild the output lut from all inputs.
 */
void ImageTFData::fillLUTFromLut(vtkLookupTablePtr output, vtkLookupTablePtr input)
{
  double b0 = mLevel-mWindow/2.0;
  double b1 = mLevel+mWindow/2.0;

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
  output->SetTableRange(b0,b1);

  for (int i=0; i<output->GetNumberOfTableValues(); ++i)
  {
    double rgba[4];
    input->GetTableValue(i, rgba);

    if (i >= llr)
      rgba[ 3 ] = 0.9999;
    else
      rgba[ 3 ] = 0.001;

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
  mLut = vtkLookupTablePtr::New(); // must reset in order to get the gpu buffering to reload
  mLut->Build();
  mLut->SetNumberOfTableValues(N);
  mLut->SetTableRange(0,N);

  vtkColorTransferFunctionPtr colorFunc = vtkColorTransferFunctionPtr::New();
  this->fillColorTFFromMap(colorFunc);

  for (int i=0; i<N; ++i)
  {
    double* rgb = colorFunc->GetColor(i+mColorMapPtr->begin()->first);
    mLut->SetTableValue(i, rgb[0], rgb[1], rgb[2], 1);
  }
  mLut->Modified();

  this->LUTChanged();
}


}
