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
	//dafault Full Range.... or level-it
//	mLevel =  0; //getScalarMax() / 2.0;
  mLevel =  getScalarMax() / 2.0;
	mWindow = getScalarMax();
	mLLR = 0.0;
	mAlpha = 1.0;

  mLevel =  getScalarMax() *0.15;
  mWindow = getScalarMax() *0.5;
//	std::cout << "mLevel: " << mLevel << ", mWindow: " << mWindow << std::endl;

	mOutputLUT = vtkLookupTablePtr::New();

	//make a default system set lookuptable, grayscale...
	vtkLookupTablePtr bwLut= vtkLookupTablePtr::New();
	bwLut->SetTableRange (0, 1);
	bwLut->SetSaturationRange (0, 0);
	bwLut->SetHueRange (0, 0);
	bwLut->SetValueRange (0, 1);
	bwLut->Build();
	this->setBaseLookupTable(bwLut);
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
  retval->mLevel = mLevel;
  retval->mWindow = mWindow;
  retval->mLLR = mLLR;
  retval->mAlpha = mAlpha;

  return retval;
}

void ImageLUT2D::setVtkImageData(vtkImageDataPtr base)
{
	mBase = base;
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
	if (similar(mLLR, val))
		return;
	mLLR = val;
	refreshOutput();
}

double ImageLUT2D::getLLR() const
{
	return mLLR;
}

void ImageLUT2D::setAlpha(double val)
{
	if (similar(mAlpha, val))
		return;
	mAlpha = val;
	std::cout << "Set Alpha :" << mAlpha << std::endl;
	refreshOutput();
}

double ImageLUT2D::getAlpha() const
{
	return mAlpha;
}

/**Set Window, i.e. the size of the intensity
 * window that will be visible.
 */
void ImageLUT2D::setWindow(double window)
{
	window = std::max(1e-5, window);

	if (similar(mWindow, window))
		return;
	mWindow = window;
	refreshOutput();
}

double ImageLUT2D::getWindow() const
{
	return mWindow;
}

/**Set Level, i.e. the position of the intensity
 * window that will be visible.
 */
void ImageLUT2D::setLevel(double level)
{
	if (similar(mLevel, level))
		return;
	mLevel = level;
	refreshOutput();
}

double ImageLUT2D::getLevel() const
{
	return mLevel;
}

/**Return the maximum intensity value of the underlying dataset.
 * The range of all settings is |0,max>
 */
double ImageLUT2D::getScalarMax() const
{
	return mBase->GetScalarRange()[1];
}

void ImageLUT2D::testMap(double val)
{
	unsigned char* color = mOutputLUT->MapValue(val);
	std::cout << "i=" << val << ", \t("
		<< static_cast<int>(color[0]) << ","
		<< static_cast<int>(color[1]) << ","
		<< static_cast<int>(color[2]) << ","
		<< static_cast<int>(color[3]) << ")" << std::endl;
}

double ImageLUT2D::mapThroughLUT(double x)
{
	double y = (mLLR - (mLevel - mWindow/2))/mWindow * mBaseLUT->GetNumberOfTableValues();
	return y;
}

/**rebuild the output lut from all inputs.
 */
void ImageLUT2D::refreshOutput()
{
	double b0 = mLevel-mWindow/2.0;
	double b1 = mLevel+mWindow/2.0;

	// find LLR on the lut:
	// We want to use the LLR on the _input_ intensity data, not on the
	// mapped data. Thus we map the llr through the lut and insert that value
	// into the lut.
	double llr = mapThroughLUT(mLLR);
	// if LLR < minimum table range, even the first value in the LUT will climb
	// above the llr. To avoid this, we use a hack that sets llr to at least 1.
	// This causes all zeros to become transparent, but the alternative is worse.
	// (what we really need is to subclass vtkLookupTable,
	//  but it contains nonvirtual functions).
	llr = std::max(1.0, llr); // hack.

	mOutputLUT->Build();
	mOutputLUT->SetNumberOfTableValues(mBaseLUT->GetNumberOfTableValues());
	mOutputLUT->SetTableRange(b0,b1);

	for (int i=0; i<mOutputLUT->GetNumberOfTableValues(); ++i)
	{
		double rgba[4];
		mBaseLUT->GetTableValue(i, rgba);

		if (i >= llr)
			rgba[ 3 ] = 0.9999;
		else
			rgba[ 3 ] = 0.001;

		mOutputLUT->SetTableValue(i, rgba);
	}
	mOutputLUT->Modified();

//	std::cout << "-----------------" << std::endl;
//	for (unsigned i=0; i<256; i+=20)
//	{
//		testMap(i);
//	}
//	testMap(255);
//	//mOutputLUT->Print(std::cout);
//	//mOutputLUT->GetTable()->Print(std::cout);
//	std::cout << "-----"
//		<< "llr=" << llr
//		<< ", LLR=" << mLLR
//		<< ", level=" << mLevel
//		<< ", window=" << mWindow
//		<< ", b=[" << b0 << "," << b1 << "] "
//		<< this
//		<< std::endl;
//	std::cout << "-----------------" << std::endl;

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
  QDomDocument doc = dataNode.ownerDocument();
  QDomElement elem = dataNode.toElement();
  //QDomElement dataNode = doc.createElement("lookuptable2D");
//  parentNode.appendChild(dataNode);
  //std::cout << "Saving window: " << mWindow << std::endl;
  elem.setAttribute("window", mWindow);
  elem.setAttribute("level", mLevel);
  elem.setAttribute("llr", mLLR);
  elem.setAttribute("alpha", mAlpha);

  //TODO: missing save of BaseLut
}

double ImageLUT2D::loadAttribute(QDomNode dataNode, QString name, double defVal)
{
  QString text = dataNode.toElement().attribute(name);
  bool ok;
  double val = text.toDouble(&ok);
  if (ok)
    return val;
  return defVal;
}

void ImageLUT2D::parseXml(QDomNode dataNode)
{
  if (dataNode.isNull())
    return;

  //std::cout << "Loading window (pre): " << mWindow << std::endl;
  mWindow = loadAttribute(dataNode, "window", mWindow);
  //std::cout << "Loading window (port): " << mWindow << std::endl;
  mLevel = loadAttribute(dataNode, "level", mLevel);
  mLLR = loadAttribute(dataNode, "llr", mLLR);
  mAlpha = loadAttribute(dataNode, "alpha", mAlpha);

  //TODO: missing load of BaseLut

  refreshOutput();
}


//---------------------------------------------------------
} // end namespace
//---------------------------------------------------------





