/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/


#ifndef CXRECONSTRUCTEDOUTPUTVOLUMEPARAMS_H_
#define CXRECONSTRUCTEDOUTPUTVOLUMEPARAMS_H_

#include "org_custusx_usreconstruction_Export.h"

#include "cxTransform3D.h"

#include "cxXmlOptionItem.h"
#include "cxTypeConversions.h"
#include "cxImageParameters.h"
#include "cxBoundingBox3D.h"

namespace cx
{

/**
 * \addtogroup org_custusx_usreconstruction
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
