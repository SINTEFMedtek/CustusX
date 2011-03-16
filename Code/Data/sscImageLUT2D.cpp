/*
 * sscImageLookupTable2D.cpp
 *
 *  Created on: Jan 9, 2009
 *      Author: christiana
 */

#include "sscImageLUT2D.h"

#include <QDomDocument>
#include <vtkLookupTable.h>
#include <vtkImageData.h>

#include "sscVector3D.h"

namespace ssc
{

ImageLUT2D::ImageLUT2D(vtkImageDataPtr base) :
	mBase(base)
{

  mData.reset(new ImageTFData());
//  double max = this->getScalarMax();
  mData->initialize(this->getScalarMax());

	//dafault Full Range.... or level-it
//	mLevel =  0; //getScalarMax() / 2.0;
//  mLevel =  getScalarMax() / 2.0;
//	mWindow = getScalarMax();
//	mLLR = 0.0;
//	mAlpha = 1.0;

  mData->setLevel(getScalarMax() * 0.15);
  mData->setWindow(getScalarMax() * 0.5);

	mOutputLUT = vtkLookupTablePtr::New();

	//make a default system set lookuptable, grayscale...
  vtkLookupTablePtr bwLut= vtkLookupTablePtr::New();
  bwLut->SetTableRange (0, 1);
  bwLut->SetSaturationRange (0, 0);
  bwLut->SetHueRange (0, 0);
  bwLut->SetValueRange (0, 1);
  bwLut->Build();
  this->setBaseLookupTable(bwLut);

  connect(mData.get(), SIGNAL(changed()), this, SIGNAL(transferFunctionsChanged()));
  connect(mData.get(), SIGNAL(changed()), this, SLOT(transferFunctionsChangedSlot()));
}

ImageLUT2DPtr ImageLUT2D::createCopy()
{
  ImageLUT2DPtr retval(new ImageLUT2D(mBase));

  retval->mOutputLUT->DeepCopy(mOutputLUT);
  if (mBaseLUT)
  {
    retval->mBaseLUT = vtkLookupTablePtr::New();
    retval->mBaseLUT->DeepCopy(mBaseLUT);
  }
  retval->mBase = mBase;

  disconnect(retval->mData.get(), SIGNAL(changed()), retval.get(), SIGNAL(transferFunctionsChanged()));
  disconnect(retval->mData.get(), SIGNAL(changed()), retval.get(), SLOT(transferFunctionsChangedSlot()));
  retval->mData = mData->createCopy();
  connect(retval->mData.get(), SIGNAL(changed()), retval.get(), SIGNAL(transferFunctionsChanged()));
  connect(retval->mData.get(), SIGNAL(changed()), retval.get(), SLOT(transferFunctionsChangedSlot()));

//  retval->mLevel = mLevel;
//  retval->mWindow = mWindow;
//  retval->mLLR = mLLR;
//  retval->mAlpha = mAlpha;

  return retval;
}

ImageTFDataPtr ImageLUT2D::getData()
{
  return mData;
}


void ImageLUT2D::setVtkImageData(vtkImageDataPtr base)
{
	mBase = base;
}

void ImageLUT2D::transferFunctionsChangedSlot()
{
  this->refreshOutput();
}

/**set basic lookuptable, to be modified by level/window/llr/alpha
 */
void ImageLUT2D::setBaseLookupTable(vtkLookupTablePtr lut)
{
	if (lut==mBaseLUT)
		return;

	mBaseLUT = lut;
	//mOutputLUT->DeepCopy(mBaseLUT);
	refreshOutput();
}

vtkLookupTablePtr ImageLUT2D::getOutputLookupTable()
{
	return mOutputLUT;
}
vtkLookupTablePtr ImageLUT2D::getBaseLookupTable()
{
	return mBaseLUT;
}
/**Set Low Level Reject, meaning the lowest intensity
 * value that will be visible.
 */
void ImageLUT2D::setLLR(double val)
{
  mData->setLLR(val);
}
double ImageLUT2D::getLLR() const
{
	return mData->getLLR();
}
void ImageLUT2D::setAlpha(double val)
{
  mData->setAlpha(val);
}
double ImageLUT2D::getAlpha() const
{
	return mData->getAlpha();
}
void ImageLUT2D::setWindow(double window)
{
  mData->setWindow(window);
}
double ImageLUT2D::getWindow() const
{
	return mData->getWindow();
}
void ImageLUT2D::setLevel(double level)
{
  mData->setLevel(level);
}
double ImageLUT2D::getLevel() const
{
	return mData->getLevel();
}

/**Return the maximum intensity value of the underlying dataset.
 * The range of all settings is |0,max>
 */
double ImageLUT2D::getScalarMax() const
{
	return mBase->GetScalarRange()[1];
}

//void ImageLUT2D::testMap(double val)
//{
//	unsigned char* color = mOutputLUT->MapValue(val);
//	std::cout << "i=" << val << ", \t("
//		<< static_cast<int>(color[0]) << ","
//		<< static_cast<int>(color[1]) << ","
//		<< static_cast<int>(color[2]) << ","
//		<< static_cast<int>(color[3]) << ")" << std::endl;
//}

/**rebuild the output lut from all inputs.
 */
void ImageLUT2D::refreshOutput()
{
//  mData->fillLUTFromLut(mOutputLUT, mBaseLUT); // sonowand way
  mData->fillLUTFromMaps(mOutputLUT); // sintef way

	emit transferFunctionsChanged();
}

///*this is a version off a llr method on Alpha channel on the lookuptable*/
//void ImageLUT2D::changeOpacity(double index_dbl, double opacity)
//{
//	int index = (int)index_dbl;
//	int noValues = mOutputLUT->GetNumberOfTableValues();
//	double scale = (getScalarMax()+1)/noValues;
//	index = (int)(index/scale);
//	index = std::max<int>(0, index);
//	index = std::min<int>(noValues, index);
//
////	if (index>noValues)
////	{
////		std::cout << "could not change opacity. index exceed size of lut ... " << std::endl;
////		return;
////	}
////	std::cout<<"set the LLR at "<<index<<std::endl;
//
//	for ( int i = 0; i < index; i++ )
//	{
//		double rgba[4];
//		mOutputLUT->GetTableValue(i, rgba);
//		rgba[ 3 ] = 0.001;
//		mOutputLUT->SetTableValue(i, rgba);
//	}
//	for ( int i = index; i < noValues; i++ )
//	{
//		double rgba[4];
//		mOutputLUT->GetTableValue(i, rgba);
//		rgba[ 3 ] = 0.9999;
//		mOutputLUT->SetTableValue(i, rgba);
//	}
//	mOutputLUT->Modified();
//}


void ImageLUT2D::addXml(QDomNode& dataNode)
{
  mData->addXml(dataNode);
//  QDomDocument doc = dataNode.ownerDocument();
//  QDomElement elem = dataNode.toElement();
//  //QDomElement dataNode = doc.createElement("lookuptable2D");
////  parentNode.appendChild(dataNode);
//  //std::cout << "Saving window: " << mWindow << std::endl;
//  elem.setAttribute("window", mWindow);
//  elem.setAttribute("level", mLevel);
//  elem.setAttribute("llr", mLLR);
//  elem.setAttribute("alpha", mAlpha);
//
//  //TODO: missing save of BaseLut
}

//double ImageLUT2D::loadAttribute(QDomNode dataNode, QString name, double defVal)
//{
//  QString text = dataNode.toElement().attribute(name);
//  bool ok;
//  double val = text.toDouble(&ok);
//  if (ok)
//    return val;
//  return defVal;
//}

void ImageLUT2D::parseXml(QDomNode dataNode)
{
  mData->parseXml(dataNode);
//  if (dataNode.isNull())
//    return;
//
//  //std::cout << "Loading window (pre): " << mWindow << std::endl;
//  mWindow = loadAttribute(dataNode, "window", mWindow);
//  //std::cout << "Loading window (port): " << mWindow << std::endl;
//  mLevel = loadAttribute(dataNode, "level", mLevel);
//  mLLR = loadAttribute(dataNode, "llr", mLLR);
//  mAlpha = loadAttribute(dataNode, "alpha", mAlpha);

//  std::map<int,QColor> input;
//  QDomNode colorNode = dataNode.namedItem("color");
//  // Read color node if it exists
//  if (!colorNode.isNull() && !colorNode.toElement().text().isEmpty())
//  {
////    mColorMapPtr->clear();
//    QStringList colorStringList = colorNode.toElement().text().split(" ");
//    for (int i = 0; i < colorStringList.size(); i++)
//    {
//      QStringList pointStringList = colorStringList[i].split("=");
//      QStringList valueStringList = pointStringList[1].split("/");
//      input[pointStringList[0].toInt()] =
//                    QColor(valueStringList[0].toInt(),
//                           valueStringList[1].toInt(),
//                           valueStringList[2].toInt());
//    }
//  }
//
//  if (!input.empty())
//  {
//    mBaseLUT->Build();
//    mBaseLUT->SetNumberOfTableValues(input.size());
//  //  mBaseLUT->SetTableRange(b0,b1);
//
//    for (int i=0; i<mBaseLUT->GetNumberOfTableValues(); ++i)
//    {
//      double rgba[4];
//      mBaseLUT->GetTableValue(i, rgba);
//
//      mBaseLUT->SetTableValue(i, rgba);
//    }
//    mBaseLUT->Modified();
//  }

  refreshOutput();
}


//---------------------------------------------------------
} // end namespace
//---------------------------------------------------------





