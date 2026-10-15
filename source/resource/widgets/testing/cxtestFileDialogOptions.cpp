/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/
#include "catch.hpp"
#include "cxFileDialogOptions.h"

namespace cxtest
{

TEST_CASE("fileDialogOptions: Disables native dialog on macOS only", "[unit][resource][widgets]")
{
	QFileDialog::Options options = cx::fileDialogOptions();
#ifdef Q_OS_MACOS
	CHECK(options.testFlag(QFileDialog::DontUseNativeDialog));
#else
	CHECK_FALSE(options.testFlag(QFileDialog::DontUseNativeDialog));
#endif
}

TEST_CASE("fileDialogOptions: Keeps given options", "[unit][resource][widgets]")
{
	QFileDialog::Options options = cx::fileDialogOptions(QFileDialog::ShowDirsOnly);
	CHECK(options.testFlag(QFileDialog::ShowDirsOnly));
}

} // namespace cxtest
