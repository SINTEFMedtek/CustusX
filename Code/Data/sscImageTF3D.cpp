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
#include "sscImageTF3D.h"

namespace ssc
{


ImageTF3D::ImageTF3D(vtkImageDataPtr base) :
	mOpacityTF(vtkPiecewiseFunctionPtr::New()),
	mColorTF(vtkColorTransferFunctionPtr::New()),	
	mBase(base)
{
  mData.reset(new ImageTFData());
  connect(mData.get(), SIGNAL(changed()), this, SIGNAL(transferFunctionsChanged()));
  connect(mData.get(), SIGNAL(changed()), this, SLOT(transferFunctionsChangedSlot()));

	double max = this->getScalarMax();
	mData->initialize(getScalarMax());
	
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

ImageTFDataPtr ImageTF3D::getData()
{
  return mData;
}

ImageTF3DPtr ImageTF3D::createCopy()
{
  ImageTF3DPtr retval(new ImageTF3D(mBase));
  retval->mOpacityTF->DeepCopy(mOpacityTF);
  retval->mColorTF->DeepCopy(mColorTF);

  disconnect(retval->mData.get(), SIGNAL(changed()), retval.get(), SIGNAL(transferFunctionsChanged()));
  disconnect(retval->mData.get(), SIGNAL(changed()), retval.get(), SLOT(transferFunctionsChangedSlot()));
  retval->mData = mData->createCopy();
  connect(retval->mData.get(), SIGNAL(changed()), retval.get(), SIGNAL(transferFunctionsChanged()));
  connect(retval->mData.get(), SIGNAL(changed()), retval.get(), SLOT(transferFunctionsChangedSlot()));

  if (mLut)
  {
    retval->mLut = vtkLookupTablePtr::New();
    retval->mLut->DeepCopy(mLut);
  }
  return retval;
}

void ImageTF3D::setVtkImageData(vtkImageDataPtr base)
{
	mBase = base;
}

vtkPiecewiseFunctionPtr ImageTF3D::getOpacityTF()
{
  if (mLut) // sonowand hack .. while we figure out how to really do this....
  {
    this->refreshOpacityTF();
    return mOpacityTF;
  }
  else
  {
    mData->fillOpacityTFFromMap(mOpacityTF);
    return mOpacityTF;
  }
}

vtkColorTransferFunctionPtr ImageTF3D::getColorTF()
{
	if (mLut) // sonowand hack .. while we figure out how to really do this....
	{
	  this->refreshColorTF();
		return mColorTF;
	}
	else
	{
	  mData->fillColorTFFromMap(mColorTF);
	  return mColorTF;
	}
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
	mLut = lut;
	this->refreshColorTF();
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
  if (!mLut)
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
	mOpacityTF->AddPoint(mData->getLLR(),          mData->getAlpha()*1/10 );
	mOpacityTF->AddPoint(mData->getLLR()+1*smooth, mData->getAlpha()*5/10 );
	mOpacityTF->AddPoint(mData->getLLR()+4*smooth, mData->getAlpha() );
#endif
	mOpacityTF->Update();
}

void ImageTF3D::addXml(QDomNode dataNode)
{
  mData->addXml(dataNode);
}

void ImageTF3D::parseXml(QDomNode dataNode)
{
  mData->parseXml(dataNode);
}

}
