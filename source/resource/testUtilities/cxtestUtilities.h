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

#ifndef CXTESTUTILITIES_H_
#define CXTESTUTILITIES_H_

#include <vector>
#include "vtkForwardDeclarations.h"
#include "cxForwardDeclarations.h"
#include "cxVector3D.h"
#include <string>
#include <QString>
#include "sscConfig.h"

namespace cxtest
{



struct TestUtilities
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
class Utilities
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

} /* namespace cxtest */
#endif /* CXTESTUTILITIES_H_ */
