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


#ifndef CXRECONSTRUCTEDOUTPUTVOLUMEPARAMS_H_
#define CXRECONSTRUCTEDOUTPUTVOLUMEPARAMS_H_

#include "org_custusx_usreconstruction_Export.h"

#include "cxTransform3D.h"
#include "cxReporter.h"
#include "cxXmlOptionItem.h"
#include "cxTypeConversions.h"
#include "cxImageParameters.h"
#include "cxBoundingBox3D.h"

namespace cx
{

/**
 * \addtogroup cx_module_usreconstruction
 * \{
 */

/** \brief Helper struct for sending and controlling output volume properties.
 *
 * \date May 27, 2010
 * \author Christian Askeland, SINTEF
 * \author Ole Vegard Solberg, SINTEF
 */
class org_custusx_usreconstruction_EXPORT OutputVolumeParams
{
public:
	OutputVolumeParams();
	OutputVolumeParams(DoubleBoundingBox3D extent, double inputSpacing, double maxVolumeSize);

	bool isValid() {return mValid;}

	unsigned long getVolumeSize() const;

	/** Set a spacing, recalculate dimensions.
	 */
	void setSpacing(double spacing);
	double getSpacing() const;
	/** Set one of the dimensions explicitly, recalculate other dims and spacing.
	 */
	void setDim(int index, int newDim);
	Eigen::Array3i getDim() const;
	/** Increase spacing in order to keep size below a max size
	 */
	void constrainVolumeSize();

	void setMaxVolumeSize(double maxSize);
	unsigned long getMaxVolumeSize();

	void set_rMd(Transform3D rMd);
	Transform3D get_rMd();

	DoubleBoundingBox3D getExtent();
	double getInputSpacing();

private:
	// controllable data, set only using the setters
	double mMaxVolumeSize;

	// constants, set only based on input data
	double mInputSpacing;

	ImageParameters mImage;
	bool mValid;

	void suggestSpacingKeepBounds(double spacing);
};

/**
 * \}
 */

} // namespace cx


#endif /* CXRECONSTRUCTEDOUTPUTVOLUMEPARAMS_H_ */
