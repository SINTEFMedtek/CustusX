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


#ifndef CXIMAGELUT2D_H_
#define CXIMAGELUT2D_H_

#include "cxResourceExport.h"
#include "cxPrecompiledHeader.h"

class QDomElement;
class QDomNode;

#include <boost/shared_ptr.hpp>
#include <QColor>
#include <QObject>
#include "vtkForwardDeclarations.h"
#include "cxImageTFData.h"

namespace cx
{
typedef boost::shared_ptr<class ImageLUT2D> ImageLUT2DPtr;


/** \brief Handling of color and opacity for 2D slices.
 *
 * Set the basic lut using either setLut() or setColorMap(), then modify it with window and level.
 * The alpha channel in this lut is ignored.
 * The opacity is controlled with LLR and Alpha values, which creates a step function for the opacity.
 * The AlphaMap from the superclass is _not_ used, it is overwritten by setting llr/alpha.
 *
 * The slicer classes will use the data either by getting the LUT+parameters(win/lvl(llr/alpha) and,
 * or by getting OutputLookupTable() which is a merge of the LUT and parameters.
 *
 * \ingroup cx_resource_core_data
 *  Created on: Jan 9, 2009
 *      Author: christiana
 */
class cxResource_EXPORT ImageLUT2D: public ImageTFData
{
Q_OBJECT
public:
	explicit ImageLUT2D();
//	void setInitialTFFromImage(vtkImageDataPtr base);
	ImageLUT2DPtr createCopy();
	void setFullRangeWinLevel(vtkImageDataPtr image); ///< Set winlevel spanning the entire range

	vtkLookupTablePtr getOutputLookupTable();

protected:
	virtual void internalsHaveChanged();

private:
	std::pair<int,int> getMapsRange();
	void buildOpacityMapFromLLRAlpha();
	void refreshOutputLUT();

	vtkLookupTablePtr mOutputLUT; ///< the sum of all internal values
};

}

#endif /* CXIMAGELUT2D_H_ */
