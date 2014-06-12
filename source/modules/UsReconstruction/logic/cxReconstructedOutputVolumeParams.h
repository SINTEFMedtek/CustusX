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

#ifndef CXRECONSTRUCTEDOUTPUTVOLUMEPARAMS_H_
#define CXRECONSTRUCTEDOUTPUTVOLUMEPARAMS_H_

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
class OutputVolumeParams
{
public:
	OutputVolumeParams();
	OutputVolumeParams(DoubleBoundingBox3D extent, double inputSpacing);

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

	void suggestSpacingKeepBounds(double spacing);
};

/**
 * \}
 */

} // namespace cx


#endif /* CXRECONSTRUCTEDOUTPUTVOLUMEPARAMS_H_ */
