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
#include "sscForwardDeclarations.h"
#include "sscVector3D.h"

namespace cxtest
{

/*class TestDataStorage
{
	static TestDataStorage* getInstance();
	static void shutdown();

	QString getDataRoot(QString suffix = "");
	void setDataRoot(QString path);

private:
	static TestDataStorage* mInstance;
	QString mDataRoot;
};*/

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
	static std::vector<cx::ImagePtr> create3DImages(unsigned int imageCount, Eigen::Array3i dim = Eigen::Array3i(3,3,3), const unsigned int voxelValue = 100);

	static unsigned int getNumberOfVoxelsAboveThreshold(vtkImageDataPtr image, int threshold);
	static unsigned int getNumberOfNonZeroVoxels(vtkImageDataPtr image);
	static double getFractionOfVoxelsAboveThreshold(vtkImageDataPtr image, int threshold);
};

} /* namespace cxtest */
#endif /* CXTESTUTILITIES_H_ */
