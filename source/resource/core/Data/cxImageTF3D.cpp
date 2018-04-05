/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/


/*
 * sscImageTF3D.cpp
 *
 *  Created on: Jan 9, 2009
 *      Author: christiana
 */

#include "cxImageTF3D.h"

#include <vector>
#include <vtkImageData.h>
#include <vtkWindowLevelLookupTable.h>
#include <vtkColorTransferFunction.h>
#include <vtkPiecewiseFunction.h>
#include <vtkVolumeProperty.h>
#include <QColor>
#include <QDomDocument>
#include <QStringList>
#include "cxVector3D.h"

#include "vtkForwardDeclarations.h"
#include "cxImageTF3D.h"

#include "cxTypeConversions.h"

namespace cx
{

ImageTF3D::ImageTF3D()
{
}

//void ImageTF3D::setInitialTFFromImage(vtkImageDataPtr base)
//{
//	double smin = base->GetScalarRange()[0];
//	double smax = base->GetScalarRange()[1];
//	double srange = smax - smin;

//	// set a winlevel spanning the entire range
////	mWindow = smax - smin;
////	mLevel = smin + mWindow / 2.0;
//	// set llr/alpha with full transmission
////	mLLR = smin;
////	mAlpha = 1.0;

////	mOpacityMap.clear();
////	this->addAlphaPoint(smin, 0);
////	this->addAlphaPoint(srange * 0.1 + smin, 0);
////	this->addAlphaPoint(smax, 255);

////	int smooth = (int) (0.1 * srange);
//	mOpacityMap.clear();
//	this->addAlphaPoint(smin - 1, 0);
//	this->addAlphaPoint(smin + 0.1  * srange, 255.0 * 0.1);
//	this->addAlphaPoint(smin + 0.4  * srange, 255.0 * 0.5);
////	this->addAlphaPoint(smin + 10 * smooth, 255.0 * 1.0);
//	this->addAlphaPoint(smax, 255);

//	mColorMap.clear();
//	this->addColorPoint(smin, Qt::black);
//	this->addColorPoint(smax, Qt::white);
//}

void ImageTF3D::internalsHaveChanged()
{
	this->refreshOpacityTF();
	this->refreshColorTF();
	emit transferFunctionsChanged();
}

ImageTF3DPtr ImageTF3D::createCopy()
{
	ImageTF3DPtr retval(new ImageTF3D());
	retval->deepCopy(this);
	return retval;
}

vtkPiecewiseFunctionPtr ImageTF3D::getOpacityTF()
{
	if (!mOpacityTF)
	{
		mOpacityTF = vtkPiecewiseFunctionPtr::New();
		this->refreshOpacityTF();
	}
	return mOpacityTF;
}

vtkColorTransferFunctionPtr ImageTF3D::getColorTF()
{
	if (!mColorTF)
	{
		mColorTF = vtkColorTransferFunctionPtr::New();
		this->refreshColorTF();
	}
	return mColorTF;
}

/**update the color TF according to the
 * mLevel, mWindow and mLut values.
 */
void ImageTF3D::refreshColorTF()
{
	if (!mColorTF)
		return;
	this->fillColorTFFromMap(mColorTF);
}

/**update the opacity TF according to the mLLR value
 */
void ImageTF3D::refreshOpacityTF()
{
	if (!mOpacityTF)
		return;
	this->fillOpacityTFFromMap(mOpacityTF);
}

/**Rebuild the opacity tf from LLR and alpha.
 * This is because the 2D renderer only handles llr+alpha.
 */
void ImageTF3D::buildOpacityMapFromLLRAlpha()
{
	// REMOVED CA 2014-02-07 - TODO
	double range = this->getWindow();
	int smooth = (int) (0.1 * range);

	mOpacityMap.clear();
//	this->addAlphaPoint(this->getScalarMin(), 0);
//	if (this->getLLR() > this->getScalarMin())
	this->addAlphaPoint(this->getLLR() - 1, 0);
	this->addAlphaPoint(this->getLLR() + 1 * smooth, this->getAlpha() * 255 * 1 / 10);
	this->addAlphaPoint(this->getLLR() + 4 * smooth, this->getAlpha() * 255 * 5 / 10);
	this->addAlphaPoint(this->getLLR() + 10 * smooth, this->getAlpha() * 255);
//	this->addAlphaPoint(this->getScalarMax(), this->getAlpha() * 255);
}

}
