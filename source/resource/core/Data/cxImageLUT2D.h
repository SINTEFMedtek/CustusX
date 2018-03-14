/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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
