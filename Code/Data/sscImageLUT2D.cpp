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
#include <vtkColorTransferFunction.h>

#include "sscVector3D.h"

namespace ssc
{

ImageLUT2D::ImageLUT2D(vtkImageDataPtr base) :
	ImageTFData(base)
{
  mOutputLUT = vtkLookupTablePtr::New();

  double smin = mBase->GetScalarRange()[0];
  double smax = mBase->GetScalarRange()[1];

  // this sets the initial opacity tf to full
  this->setAlpha(1);
  this->setLLR(smin);
  this->buildOpacityMapFromLLRAlpha();

  // this also sets the initial lut to grayscale
  this->addColorPoint(smin, Qt::black);
  this->addColorPoint(smax, Qt::white);

  // set values suitable for CT.
  this->setLevel(smax * 0.15);
  this->setWindow(smax * 0.5);

  connect(this, SIGNAL(changed()), this, SIGNAL(transferFunctionsChanged()));
  connect(this, SIGNAL(changed()), this, SLOT(transferFunctionsChangedSlot()));
}

ImageLUT2DPtr ImageLUT2D::createCopy()
{
  ImageLUT2DPtr retval(new ImageLUT2D(mBase));

  retval->deepCopy(this);
  retval->mOutputLUT->DeepCopy(mOutputLUT);

  return retval;
}

void ImageLUT2D::transferFunctionsChangedSlot()
{
  this->refreshOutput();
}

/**set basic lookuptable, to be modified by level/window/llr/alpha
 */
void ImageLUT2D::setBaseLookupTable(vtkLookupTablePtr lut)
{
  this->setLut(lut);
}

vtkLookupTablePtr ImageLUT2D::getOutputLookupTable()
{
	return mOutputLUT;
}
vtkLookupTablePtr ImageLUT2D::getBaseLookupTable()
{
  return this->getLut();
}

void ImageLUT2D::LUTChanged()
{
  this->refreshOutput();
}

void ImageLUT2D::alphaLLRChanged()
{
  this->buildOpacityMapFromLLRAlpha();
}

/**Rebuild the opacity tf from LLR and alpha.
 * This is because the 2D renderer only handles llr+alpha.
 */
void ImageLUT2D::buildOpacityMapFromLLRAlpha()
{
  mOpacityMapPtr->clear();
  this->addAlphaPoint(this->getScalarMin(),0);
  if (this->getLLR()>this->getScalarMin())
    this->addAlphaPoint(this->getLLR()-1,0);
  this->addAlphaPoint(this->getLLR(),this->getAlpha()*255);
  this->addAlphaPoint(this->getScalarMax(),this->getAlpha()*255);
}

/**rebuild the output lut from all inputs.
 */
void ImageLUT2D::refreshOutput()
{
  this->fillLUTFromLut(mOutputLUT, mLut);
	emit transferFunctionsChanged();
}

void ImageLUT2D::addXml(QDomNode dataNode)
{
  ImageTFData::addXml(dataNode);
}

void ImageLUT2D::parseXml(QDomNode dataNode)
{
  ImageTFData::parseXml(dataNode);

  this->buildLUTFromColorMap();
  this->refreshOutput();
}


//---------------------------------------------------------
} // end namespace
//---------------------------------------------------------





