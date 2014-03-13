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

#ifndef CXTRANSFORMFILE_H_
#define CXTRANSFORMFILE_H_

#include "sscTransform3D.h"

namespace cx
{
/**
 * \file
 * \addtogroup cx_resource_core_utilities
 * @{
 */

/**
 * \brief File format for storing a 4x4 matrix.
 * \ingroup cxResourceUtilities
 *
 * The read/write methods emit error messages if you
 * dont use the ok flag.
 *
 * \date Feb 28, 2012
 * \author Christian Askeland, SINTEF
 */
class TransformFile
{
public:
	TransformFile(QString fileName ="");
	QString fileName() const { return mFileName; }
	Transform3D read(bool* ok = 0);
	void write(const Transform3D& transform);

private:
	QString mFileName;
};

/**
 * @}
 */

}

#endif /* CXTRANSFORMFILE_H_ */
