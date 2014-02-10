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

#ifndef CXIMAGEDEFAULTTFGENERATOR_H
#define CXIMAGEDEFAULTTFGENERATOR_H

#include "cxForwardDeclarations.h"

namespace cx
{

/** Generates default transfer functions for an Image.
 *
 * Based on information on scalar range, modality,
 * and default windowlevel.
 *
 * \date 2014-02-10
 * \author christiana
 */
class ImageDefaultTFGenerator
{
public:
	ImageDefaultTFGenerator(ImagePtr image);
	ImageLUT2DPtr generate2DTFPreset();
	ImageTF3DPtr generate3DTFPreset();
	void resetShading();

private:
	ImagePtr mImage;
	std::pair<double, double> guessInitialScalarRange();
	std::pair<double, double> getFullScalarRange();
	std::pair<double, double> getInitialWindowRange();
	bool hasValidInitialWindow() const;
	std::pair<double, double> guessMRRange();
	std::pair<double, double> guessCTRange();

};


} // namespace cx


#endif // CXIMAGEDEFAULTTFGENERATOR_H
