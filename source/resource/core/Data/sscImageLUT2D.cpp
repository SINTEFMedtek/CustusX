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
#include <vtkPiecewiseFunction.h>

#include "sscVector3D.h"
#include "sscLogger.h"

namespace cx
{

ImageLUT2D::ImageLUT2D()
{
}

void ImageLUT2D::setInitialTFFromImage(vtkImageDataPtr base)
{
	double smin = base->GetScalarRange()[0];
	double smax = base->GetScalarRange()[1];
	double srange = smax - smin;

	// this sets the initial opacity tf to full
	mOpacityMap.clear();
	this->addAlphaPoint(smin - 1, 0);
	this->addAlphaPoint(smin, 255);

	// this also sets the initial lut to grayscale
	mColorMap.clear();
	this->addColorPoint(smin, Qt::black);
	this->addColorPoint(smax, Qt::white);

//	if (base->GetNumberOfScalarComponents() <=2)
//	{
//		// set values suitable for CT.
//	//	this->setLevel(srange * 0.15 + smin);
//	//	this->setWindow(srange * 0.5);
//		// changed default values (2012.03.29-CA) : the previous was usually a bad guess, especially for MR. Use almost entire range instead
//		this->setLevel(smin + 0.8*srange * 0.5);
//		this->setWindow(0.8*srange);
//	}
//	else
//	{
//		// set full range for color images. Assume they want to be rendered as is.
//		this->setLevel(smin + srange * 0.5);
//		this->setWindow(srange);
//	}

	this->internalsHaveChanged();
}

ImageLUT2DPtr ImageLUT2D::createCopy()
{
	ImageLUT2DPtr retval(new ImageLUT2D());
	retval->deepCopy(this);
	return retval;
}

void ImageLUT2D::setFullRangeWinLevel(vtkImageDataPtr image)
{
	double smin = image->GetScalarRange()[0];
	double smax = image->GetScalarRange()[1];
	double srange = smax - smin;
	this->setWindow(srange);
	this->setLevel(smin + srange / 2.0);
}

vtkLookupTablePtr ImageLUT2D::getOutputLookupTable()
{
	if (!mOutputLUT)
	{
		mOutputLUT = vtkLookupTablePtr::New();
		this->refreshOutputLUT();
	}
	return mOutputLUT;
}

///**Rebuild the opacity tf from LLR and alpha.
// * This is because the 2D renderer only handles llr+alpha.
// */
//void ImageLUT2D::buildOpacityMapFromLLRAlpha()
//{
//	// REMOVED CA 2014-02-07 - TODO
//	mOpacityMap.clear();
////	this->addAlphaPoint(this->getScalarMin(), 0);
////	if (this->getLLR() > this->getScalarMin())
//		this->addAlphaPoint(this->getLLR() - 1, 0);
//	this->addAlphaPoint(this->getLLR(), this->getAlpha() * 255);
////	this->addAlphaPoint(this->getScalarMax(), this->getAlpha() * 255);
//}

void ImageLUT2D::internalsHaveChanged()
{
	this->refreshOutputLUT();
	emit transferFunctionsChanged();
}

std::pair<int,int> ImageLUT2D::getMapsRange()
{
	std::pair<int,int> retval;

	int imin = 0;
	int imax = 0;

	if (!mColorMap.empty() && !mOpacityMap.empty())
	{
		int imin = std::min(mColorMap.begin()->first, mOpacityMap.begin()->first);
		int imax = std::max(mColorMap.rbegin()->first, mOpacityMap.rbegin()->first);
		return std::make_pair(imin,imax);
	}
	else if (!mColorMap.empty())
	{
		int imin = mColorMap.begin()->first;
		int imax = mColorMap.rbegin()->first;
		return std::make_pair(imin,imax);
	}
	else if (!mOpacityMap.empty())
	{
		int imin = mOpacityMap.begin()->first;
		int imax = mOpacityMap.rbegin()->first;
		return std::make_pair(imin,imax);
	}
	else
	{
		return std::make_pair(0,0);
	}
}

void ImageLUT2D::refreshOutputLUT()
{
	if (!mOutputLUT)
		return;

	std::pair<int,int> range = this->getMapsRange();
	int imin = range.first;
	int imax = range.second;
	if (imin==imax)
		imax = imin+1;
	int icount = imax - imin + 1;

	vtkLookupTablePtr lut = mOutputLUT;
	lut->Build();
	lut->SetNumberOfTableValues(icount);
	lut->SetTableRange(imin, imax);

	vtkColorTransferFunctionPtr colorFunc = this->generateColorTF();
	vtkPiecewiseFunctionPtr opacityFunc = this->generateOpacityTF();

	for (int i = 0; i < icount; ++i)
	{
		double* rgb = colorFunc->GetColor(i + imin);
		double alpha = opacityFunc->GetValue(i + imin);
		lut->SetTableValue(i, rgb[0], rgb[1], rgb[2], alpha);
	}

	lut->Modified();
}

//---------------------------------------------------------
} // end namespace
//---------------------------------------------------------


