/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#ifndef CXFILEDIALOGOPTIONS_H
#define CXFILEDIALOGOPTIONS_H

#include <QFileDialog>

namespace cx
{

/** Add platform-specific QFileDialog options.
 *  On macOS the native dialog may silently fail to open, so Qt's own dialog is used there.
 */
inline QFileDialog::Options fileDialogOptions(QFileDialog::Options options = QFileDialog::Options())
{
#ifdef Q_OS_MACOS
	options |= QFileDialog::DontUseNativeDialog;
#endif
	return options;
}

} // namespace cx

#endif // CXFILEDIALOGOPTIONS_H
