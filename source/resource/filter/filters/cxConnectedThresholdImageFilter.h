/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXCONNECTEDTHRESHOLDIMAGEFILTER_H_
#define CXCONNECTEDTHRESHOLDIMAGEFILTER_H_

#include "cxThreadedTimedAlgorithm.h"
#include "cxResourceFilterExport.h"
#include "cxVector3D.h"
#include "vtkForwardDeclarations.h"

namespace cx
{
typedef boost::shared_ptr<class VisServices> VisServicesPtr;

/**
 * \file
 * \addtogroup cx_resource_filter
 * @{
 */

/**
 * \class ConnectedThresholdImageFilter
 *
 * \brief Segmenting using region growing.
 *
 * \warning Class used for course, not tested.
 *
 * \date Apr 26, 2011
 * \author Janne Beate Bakeng, SINTEF
 */
class cxResourceFilter_EXPORT ConnectedThresholdImageFilter : public ThreadedTimedAlgorithm<vtkImageDataPtr>
{
	Q_OBJECT

public:
	ConnectedThresholdImageFilter(VisServicesPtr services);
	virtual ~ConnectedThresholdImageFilter();

	void setInput(ImagePtr image, QString outputBasePath, float lowerThreshold, float upperThreshold, int replaceValue, Eigen::Array3i seed);
	/** Voxels in [lower, upper] that are face connected to the seed voxel get replaceValue, the rest 0, as short.
	 * Values are converted to short first. Returns null if the seed is outside the image. */
	static vtkImageDataPtr segment(vtkImageDataPtr image, double lower, double upper, int replaceValue, Eigen::Array3i seed);
	virtual void execute() { throw "not implemented!!"; }
	ImagePtr getOutput();

private slots:
	virtual void postProcessingSlot();

private:
	virtual vtkImageDataPtr calculate();

	VisServicesPtr mServices;
	QString       mOutputBasePath;
	ImagePtr mInput;
	ImagePtr mOutput;

	float           mLowerThreshold;
	float           mUpperTheshold;
	int             mReplaceValue;
	Eigen::Array3i mSeed;
};

/**
 * @}
 */
}

#endif /* CXCONNECTEDTHRESHOLDIMAGEFILTER_H_ */
