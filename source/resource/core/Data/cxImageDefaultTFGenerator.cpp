/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without 
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors 
   may be used to endorse or promote products derived from this software 
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE 
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER 
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, 
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
=========================================================================*/

#include "cxImageDefaultTFGenerator.h"

#include <cmath>
#include "vtkImageData.h"
#include "cxImage.h"
#include "cxImageLUT2D.h"
#include "cxImageTF3D.h"
#include "math.h"
#include "cxSettings.h"

namespace cx
{

ImageDefaultTFGenerator::ImageDefaultTFGenerator(ImagePtr image) : mImage(image)
{

}

void ImageDefaultTFGenerator::resetShading()
{
	// add shading for known preoperative modalities
	if (mImage->getModality().contains("CT") || mImage->getModality().contains("MR"))
		mImage->setShadingOn(settings()->value("View/shadingOn").value<bool>());
}

ImageLUT2DPtr ImageDefaultTFGenerator::generate2DTFPreset()
{
	ImageLUT2DPtr tf(new ImageLUT2D());

	double_pair fullRange = this->getFullScalarRange();
	double_pair range = this->guessInitialScalarRange();
	double smin = range.first;
	double smax = range.second;

	IntIntMap opacity;
	opacity[fullRange.first - 1] = 0;
	opacity[fullRange.first] = 255;
	tf->resetAlpha(opacity);

	ColorMap colors;
	colors[smin] = QColor(Qt::black);
	colors[smax] = QColor(Qt::white);
	tf->resetColor(colors);

	return tf;
}

namespace
{
int myround(double val)
{
	return int(val+0.5);
}
}

ImageTF3DPtr ImageDefaultTFGenerator::generate3DTFPreset()
{
	ImageTF3DPtr tf(new ImageTF3D());

	double_pair range = this->guessInitialScalarRange();

	double smin = range.first;
	double smax = range.second;
	double srange = smax - smin;

	IntIntMap opacity;
	// Note the ordering: add in descending order to ensure zero is
	// always written into smin, also for binary volumes
	// Round is required for binary volumes.
	opacity[smin + myround(0.5*srange)] = 255;
	opacity[smin + myround(0.3*srange)] = 255.0 * 0.7;
	opacity[smin + myround(0.1*srange)] = 0;
	tf->resetAlpha(opacity);

	ColorMap colors;
	colors[smin] = QColor(Qt::black);
	colors[smax] = QColor(Qt::white);
	tf->resetColor(colors);

	return tf;
}

bool ImageDefaultTFGenerator::hasValidInitialWindow() const
{
	return mImage->getInitialWindowWidth()>0;
}

double_pair ImageDefaultTFGenerator::guessInitialScalarRange() const
{
	double_pair srange = this->getFullScalarRange();

	if (this->hasValidInitialWindow())
	{
		srange = this->getInitialWindowRange();
	}
	// no initial window: add heuristics for each modality
	if (!this->hasValidInitialWindow())
	{
		if (this->isUnsignedChar())
		{
			srange.first = 0;
			srange.second = 255;

			if (this->looksLikeBinaryImage())
			{
				srange.first = 0;
				srange.second = 1;
			}
		}
		if (mImage->getModality().contains("CT"))
		{
			srange = this->guessCTRange();
		}
		if (mImage->getModality().contains("MR"))
		{
			srange = this->guessMRRange();
		}
	}

	srange = this->ensureNonZeroRoundedRange(srange);
	return srange;
}

bool ImageDefaultTFGenerator::isUnsignedChar() const
{
	return mImage->getBaseVtkImageData()->GetScalarType() == VTK_UNSIGNED_CHAR;
}

bool ImageDefaultTFGenerator::looksLikeBinaryImage() const
{
	double_pair r = this->getFullScalarRange();
	return similar(r.first, 0) && similar(r.second, 1);
}


double_pair ImageDefaultTFGenerator::ensureNonZeroRoundedRange(double_pair range) const
{
	range.first = std::round(range.first);
	range.second = std::round(range.second);
	range.second = std::max(range.second, range.first+1);
	return range;
}

double_pair ImageDefaultTFGenerator::getFullScalarRange() const
{
	double smin = mImage->getBaseVtkImageData()->GetScalarRange()[0];
	double smax = mImage->getBaseVtkImageData()->GetScalarRange()[1];
	return std::make_pair(smin, smax);
}

double_pair ImageDefaultTFGenerator::getInitialWindowRange() const
{
	double smin = mImage->getInitialWindowLevel() - mImage->getInitialWindowWidth()/2;
	double smax = mImage->getInitialWindowLevel() + mImage->getInitialWindowWidth()/2;
	return std::make_pair(smin, smax);
}

double_pair ImageDefaultTFGenerator::guessCTRange() const
{
	// signed: [-1024...3072]
	// choose a default from lung to bone, approximately.
	double smin = -500;
	double smax = 900;
	if (0 >= mImage->getMin()) // unsigned: [0..4096]
	{
		int ct_signed2unsigned = 1024;
		smin += ct_signed2unsigned;
		smax += ct_signed2unsigned;
	}
	return std::make_pair(smin, smax);
}

double_pair ImageDefaultTFGenerator::guessMRRange() const
{
	double_pair srange = this->getFullScalarRange();
	srange.second *= 0.25; // usually lots of high-intensity noise of no interest
	return srange;
}


} // namespace cx
