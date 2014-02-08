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
#include "sscLogger.h"
#include "sscTypeConversions.h"

namespace cx
{

ImageTF3D::ImageTF3D() //:
//	ImageTFData(base)//, mOpacityTF(vtkPiecewiseFunctionPtr::New()), mColorTF(vtkColorTransferFunctionPtr::New())
{
//	mColorTF->SetColorSpaceToRGB();

//	double smin = base->GetScalarRange()[0];
//	double smax = base->GetScalarRange()[1];
//	double srange = smax - smin;

//	//  double max = this->getScalarMax();
//	//	mOpacityTF->AddPoint(0.0, 0.0);
//	//	mOpacityTF->AddPoint(max, 1.0);
//	//	mColorTF->AddRGBPoint(0.0, 0.0, 0.0, 0.0);
//	//	mColorTF->AddRGBPoint(max, 1.0, 1.0, 1.0);

//	this->addAlphaPoint(smin, 0);
//	this->addAlphaPoint(srange * 0.1 + smin, 0);
//	this->addAlphaPoint(smax, 255);

//	this->addColorPoint(smin, Qt::black);
//	this->addColorPoint(smax, Qt::white);

	connect(this, SIGNAL(changed()), this, SIGNAL(transferFunctionsChanged()));
	connect(this, SIGNAL(changed()), this, SLOT(transferFunctionsChangedSlot()));
}

void ImageTF3D::setInitialTFFromImage(vtkImageDataPtr base)
{
	double smin = base->GetScalarRange()[0];
	double smax = base->GetScalarRange()[1];
	double srange = smax - smin;

	// set a winlevel spanning the entire range
	mWindow = smax - smin;
	mLevel = smin + mWindow / 2.0;
	// set llr/alpha with full transmission
	mLLR = smin;
	mAlpha = 1.0;

	//  double max = this->getScalarMax();
	//	mOpacityTF->AddPoint(0.0, 0.0);
	//	mOpacityTF->AddPoint(max, 1.0);
	//	mColorTF->AddRGBPoint(0.0, 0.0, 0.0, 0.0);
	//	mColorTF->AddRGBPoint(max, 1.0, 1.0, 1.0);

	this->addAlphaPoint(smin, 0);
	this->addAlphaPoint(srange * 0.1 + smin, 0);
	this->addAlphaPoint(smax, 255);

	this->addColorPoint(smin, Qt::black);
	this->addColorPoint(smax, Qt::white);
}


//void ImageTF3D::removeInitAlphaPoint()
//{
//	double smin = mBase->GetScalarRange()[0];
//	double smax = mBase->GetScalarRange()[1];
//	double srange = smax - smin;
//	this->removeAlphaPoint(srange * 0.1 + smin);
//}

void ImageTF3D::transferFunctionsChangedSlot()
{
	this->refreshOpacityTF();
	this->refreshColorTF();
}

ImageTF3DPtr ImageTF3D::createCopy()
{
	ImageTF3DPtr retval(new ImageTF3D());
	retval->deepCopy(this);
//	retval->setVtkImageData(newDataBase);//deepCopy also copies the data base
	return retval;
}

vtkPiecewiseFunctionPtr ImageTF3D::getOpacityTF()
{
	this->refreshOpacityTF();
	return mOpacityTF;
}

vtkColorTransferFunctionPtr ImageTF3D::getColorTF()
{
	this->refreshColorTF();
	return mColorTF;
}

void ImageTF3D::LUTChanged()
{
	this->refreshColorTF();
	emit transferFunctionsChanged();
}

/**update the color TF according to the
 * mLevel, mWindow and mLut values.
 */
void ImageTF3D::refreshColorTF()
{
	if (!mColorTF)
	{
		mColorTF = vtkColorTransferFunctionPtr::New();
		mColorTF->SetColorSpaceToRGB();
	}

	this->fillColorTFFromMap(mColorTF);
}

/**update the opacity TF according to the mLLR value
 */
void ImageTF3D::refreshOpacityTF()
{
//	if (!mLut)
//		return;

	if (!mOpacityTF)
	{
		mOpacityTF = vtkPiecewiseFunctionPtr::New();
	}
	this->fillOpacityTFFromMap(mOpacityTF);
}

void ImageTF3D::alphaLLRChanged()
{
	this->buildOpacityMapFromLLRAlpha();
}

/**Rebuild the opacity tf from LLR and alpha.
 * This is because the 2D renderer only handles llr+alpha.
 */
void ImageTF3D::buildOpacityMapFromLLRAlpha()
{
	// REMOVED CA 2014-02-07 - TODO
	double range = this->getWindow();
	int smooth = (int) (0.1 * range);

	mOpacityMapPtr->clear();
//	this->addAlphaPoint(this->getScalarMin(), 0);
//	if (this->getLLR() > this->getScalarMin())
	this->addAlphaPoint(this->getLLR() - 1, 0);
	this->addAlphaPoint(this->getLLR() + 1 * smooth, this->getAlpha() * 255 * 1 / 10);
	this->addAlphaPoint(this->getLLR() + 4 * smooth, this->getAlpha() * 255 * 5 / 10);
	this->addAlphaPoint(this->getLLR() + 10 * smooth, this->getAlpha() * 255);
//	this->addAlphaPoint(this->getScalarMax(), this->getAlpha() * 255);
}

void ImageTF3D::addXml(QDomNode dataNode)
{
	ImageTFData::addXml(dataNode);
}

void ImageTF3D::parseXml(QDomNode dataNode)
{
	ImageTFData::parseXml(dataNode);
}

}
