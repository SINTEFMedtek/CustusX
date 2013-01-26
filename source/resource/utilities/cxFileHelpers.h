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
#ifndef CXFILEHELPERS_H
#define CXFILEHELPERS_H

#include <QString>

namespace cx
{

/**
 * \addtogroup cxResourceUtilities
 * \{
 */

/** Remove a nonempty folder recursively
 *
 * http://stackoverflow.com/questions/11050977/removing-a-non-empty-folder-in-qt
 *
 * \ingroup cxResourceUtilities
 * \date jan 26, 2013
 * \author christiana
 */
bool removeNonemptyDirRecursively(const QString& dirName);

/**
 * \}
 */

} // namespace cx


#endif // CXFILEHELPERS_H
