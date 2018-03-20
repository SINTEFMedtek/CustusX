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
#include "cxAlgorithmHelpers.h"

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

	void setInput(ImagePtr image, QString outputBasePath, float lowerThreshold, float upperThreshold, int replaceValue, itkImageType::IndexType seed);
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
	itkImageType::IndexType mSeed;
};

/**
 * @}
 */
}

#endif /* CXCONNECTEDTHRESHOLDIMAGEFILTER_H_ */
