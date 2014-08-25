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

#ifndef CXIMAGEDEFAULTTFGENERATOR_H
#define CXIMAGEDEFAULTTFGENERATOR_H

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
class ImageDefaultTFGenerator
{
public:
	ImageDefaultTFGenerator(ImagePtr image);
	ImageLUT2DPtr generate2DTFPreset();
	ImageTF3DPtr generate3DTFPreset();
	void resetShading();

private:
	ImagePtr mImage;
	double_pair guessInitialScalarRange();
	double_pair getFullScalarRange();
	double_pair getInitialWindowRange();
	bool hasValidInitialWindow() const;
	double_pair guessMRRange();
	double_pair guessCTRange();
	double_pair ensureNonZeroRoundedRange(double_pair range);
	bool isUnsignedChar() const;
};


} // namespace cx


#endif // CXIMAGEDEFAULTTFGENERATOR_H
