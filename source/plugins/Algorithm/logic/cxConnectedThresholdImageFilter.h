#ifndef CXCONNECTEDTHRESHOLDIMAGEFILTER_H_
#define CXCONNECTEDTHRESHOLDIMAGEFILTER_H_

#include "cxThreadedTimedAlgorithm.h"
#include "cxAlgorithmHelpers.h"
#include "cxLegacySingletons.h"

namespace cx
{
/**
 * \file
 * \addtogroup cxPluginAlgorithm
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
class ConnectedThresholdImageFilter : public ThreadedTimedAlgorithm<vtkImageDataPtr>
{
	Q_OBJECT

public:
	ConnectedThresholdImageFilter();
	virtual ~ConnectedThresholdImageFilter();

	void setInput(ImagePtr image, QString outputBasePath, float lowerThreshold, float upperThreshold, int replaceValue, itkImageType::IndexType seed);
	virtual void execute() { throw "not implemented!!"; }
	ImagePtr getOutput();

private slots:
	virtual void postProcessingSlot();

private:
	virtual vtkImageDataPtr calculate();

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
