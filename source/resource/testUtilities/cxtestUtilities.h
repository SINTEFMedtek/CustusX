/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXTESTUTILITIES_H_
#define CXTESTUTILITIES_H_

#include "cxtestutilities_export.h"

#include <vector>
#include "vtkForwardDeclarations.h"
#include "cxForwardDeclarations.h"
#include "cxVector3D.h"
#include <string>
#include <QString>
#include "sscConfig.h"

namespace cxtest
{

struct CXTESTUTILITIES_EXPORT TestUtilities
{
	static inline QString GetDataRoot();
	static inline QString ExpandDataFileName(QString fileName);
};

inline
QString TestUtilities::GetDataRoot()
{
	QString dataRoot = SSC_DATA_ROOT;
	return dataRoot;
}

inline
QString TestUtilities::ExpandDataFileName(QString fileName)
{
	QString dataRoot = SSC_DATA_ROOT;
	QString expandedName = dataRoot + fileName;
	return expandedName;
}

/**
 * A collection of functionality used for testing.
 *
 * \date May 21, 2013
 * \author Janne Beate Bakeng, SINTEF
 */
class CXTESTUTILITIES_EXPORT Utilities
{
public:
	static QString getDataRoot(QString suffix = "");
	static vtkImageDataPtr create3DVtkImageData(Eigen::Array3i dim = Eigen::Array3i(3,3,3), const unsigned int voxelValue = 100);
	static cx::ImagePtr create3DImage(Eigen::Array3i dim = Eigen::Array3i(3,3,3), const unsigned int voxelValue = 100);
	static cx::ImagePtr create3DImage(Eigen::Array3i dim, cx::Vector3D spacing, const unsigned int voxelValue);
	static std::vector<cx::ImagePtr> create3DImages(unsigned int imageCount, Eigen::Array3i dim = Eigen::Array3i(3,3,3), const unsigned int voxelValue = 100);

	static unsigned int getNumberOfVoxelsAboveThreshold(vtkImageDataPtr image, int threshold, int component=0);
	static unsigned int getNumberOfNonZeroVoxels(vtkImageDataPtr image);
	static double getFractionOfVoxelsAboveThreshold(vtkImageDataPtr image, int threshold, int component=0);

	static void sleep_sec(int seconds);
};

//To link the executable (Catch) which runs the tests on Windows, each test lib must contain at least one class to be exported.
//If your test lib doesn't contain any classes which must be exported, this macro can be used to create a dummy class in a uniform way across the code base.
//Put this macro in a cpp file in your test folder. To make it more visible the name cxtestExportDummyClassOnWindowsForLinking.cpp has been used for such files.
//Remember to include the export header file and then use this macro with the export macro for your lib.
//Example:
// #include "cxtest_org_custusx_registration_method_manual_export.h"
//
// namespace
// {
// EXPORT_DUMMY_CLASS_FOR_LINKING_ON_WINDOWS_IN_LIB_WITHOUT_EXPORTED_CLASS(CXTEST_ORG_CUSTUSX_REGISTRATION_METHOD_MANUAL_EXPORT)
// }
#define EXPORT_DUMMY_CLASS_FOR_LINKING_ON_WINDOWS_IN_LIB_WITHOUT_EXPORTED_CLASS(EXPORT_MACRO) \
class EXPORT_MACRO DummyTestClassForLinkingOnWindows \
{ \
};

} /* namespace cxtest */
#endif /* CXTESTUTILITIES_H_ */
