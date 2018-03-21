/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXIMAGEDEFAULTTFGENERATOR_H
#define CXIMAGEDEFAULTTFGENERATOR_H

#include "cxResourceExport.h"
#include "cxPrecompiledHeader.h"

#include "cxForwardDeclarations.h"

namespace cx
{
typedef std::pair<double,double> double_pair;


/** Generates default transfer functions for an Image.
 *
 * Based on information on scalar range, modality,
 * and default windowlevel.
 *
 * \ingroup cx_resource_core_data
 * \date 2014-02-10
 * \author christiana
 */
class cxResource_EXPORT ImageDefaultTFGenerator
{
public:
	ImageDefaultTFGenerator(ImagePtr image);
	ImageLUT2DPtr generate2DTFPreset();
	ImageTF3DPtr generate3DTFPreset();
	void resetShading();

private:
	ImagePtr mImage;
	double_pair guessInitialScalarRange() const;
	double_pair getFullScalarRange() const;
	double_pair getInitialWindowRange() const;
	bool hasValidInitialWindow() const;
	double_pair guessMRRange() const;
	double_pair guessCTRange() const;
	double_pair ensureNonZeroRoundedRange(double_pair range) const;
	bool isUnsignedChar() const;
	bool looksLikeBinaryImage() const;
};


} // namespace cx


#endif // CXIMAGEDEFAULTTFGENERATOR_H
