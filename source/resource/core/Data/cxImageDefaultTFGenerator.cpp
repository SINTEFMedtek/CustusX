// This file is part of CustusX, an Image Guided Therapy Application.
//
// Copyright (C) 2008- SINTEF Technology & Society, Medical Technology
//
// CustusX is fully owned by SINTEF Medical Technology (SMT). CustusX source
// code and binaries can only be used by SMT and those with explicit permission
// from SMT. CustusX shall not be distributed to anyone else.
//
// CustusX is a research tool. It is NOT intended for use or certified for use
// in a normal clinical setting. SMT does not take responsibility for its use
// in any way.
//
// See CustusX_License.txt for more information.

#include "cxImageDefaultTFGenerator.h"

#include "vtkImageData.h"
#include "cxImage.h"
#include "cxImageLUT2D.h"
#include "cxImageTF3D.h"

namespace cx
{

ImageDefaultTFGenerator::ImageDefaultTFGenerator(ImagePtr image) : mImage(image)
{

}

void ImageDefaultTFGenerator::resetShading()
{
	// add shading for known preoperative modalities
	if (mImage->getModality().contains("CT") || mImage->getModality().contains("MR"))
		mImage->setShadingOn(true);
}

ImageLUT2DPtr ImageDefaultTFGenerator::generate2DTFPreset()
{
	ImageLUT2DPtr tf(new ImageLUT2D());

	double_pair fullRange = this->getFullScalarRange();
	double_pair range = this->guessInitialScalarRange();
	double smin = range.first;
	double smax = range.second;
	double srange = smax - smin;
//	std::cout << "TF2D: " << smin << ", " << smax << std::endl;

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

ImageTF3DPtr ImageDefaultTFGenerator::generate3DTFPreset()
{
	ImageTF3DPtr tf(new ImageTF3D());

	double_pair range = this->guessInitialScalarRange();
//	std::cout << "    end: " << range.first << ", " << range.second << std::endl;

	double smin = range.first;
	double smax = range.second;
	double srange = smax - smin;
//	std::cout << "TF3D: " << smin << ", " << smax << std::endl;

	IntIntMap opacity;
//	opacity[smin - 1] = 0;
	// Note the ordering: add in descending order to ensure zero is
	// always written into smin, also for binary volumes
	opacity[smax             ] = 255;
	opacity[smin + 0.5*srange] = 255;
	opacity[smin + 0.3*srange] = 255.0 * 0.7;
	opacity[smin + 0.1*srange] = 0;
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

double_pair ImageDefaultTFGenerator::guessInitialScalarRange()
{
	double_pair srange = this->getFullScalarRange();

	if (this->hasValidInitialWindow())
	{
		srange = this->getInitialWindowRange();
	}
	// no initial window: add heuristics for each modality
	if (!this->hasValidInitialWindow())
	{
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

double_pair ImageDefaultTFGenerator::ensureNonZeroRoundedRange(double_pair range)
{
	range.first = int(range.first+0.5);
	range.second = int(range.second+0.5);
	range.second = std::max(range.second, range.first+1);
	return range;
}

double_pair ImageDefaultTFGenerator::getFullScalarRange()
{
	double smin = mImage->getBaseVtkImageData()->GetScalarRange()[0];
	double smax = mImage->getBaseVtkImageData()->GetScalarRange()[1];
//	std::cout << "    basic: " << smin << ", " << smax << std::endl;
	return std::make_pair(smin, smax);
}

double_pair ImageDefaultTFGenerator::getInitialWindowRange()
{
	double smin = mImage->getInitialWindowLevel() - mImage->getInitialWindowWidth()/2;
	double smax = mImage->getInitialWindowLevel() + mImage->getInitialWindowWidth()/2;
//	std::cout << "    initial: " << smin << ", " << smax << std::endl;
	return std::make_pair(smin, smax);
}

double_pair ImageDefaultTFGenerator::guessCTRange()
{
	// signed: [-1024...3072]
	// choose a default from lung to bone, approximately.
	double smin = -500;
	double smax = 900;
//	std::cout << "    signedCT: " << smin << ", " << smax << std::endl;
	if (0 >= mImage->getMin()) // unsigned: [0..4096]
	{
		int ct_signed2unsigned = 1024;
		smin += ct_signed2unsigned;
		smax += ct_signed2unsigned;
//		std::cout << "    unsignedCT: " << smin << ", " << smax << std::endl;
	}
	return std::make_pair(smin, smax);
}

double_pair ImageDefaultTFGenerator::guessMRRange()
{
	double_pair srange = this->getFullScalarRange();
	srange.second *= 0.25; // usually lots of high-intensity noise of no interest
//	std::cout << "    MR: " << srange.first << ", " << srange.second << std::endl;
	return srange;
}


} // namespace cx
