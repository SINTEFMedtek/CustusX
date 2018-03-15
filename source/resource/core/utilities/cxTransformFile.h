/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXTRANSFORMFILE_H_
#define CXTRANSFORMFILE_H_

#include "cxResourceExport.h"

#include "cxTransform3D.h"

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
class cxResource_EXPORT TransformFile
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
