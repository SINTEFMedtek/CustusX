#ifndef CXSIMULATEDIMAGESTREAMER_H_
#define CXSIMULATEDIMAGESTREAMER_H_

#include "cxImageStreamer.h"
#include "sscTransform3D.h"

namespace cx
{
typedef ssc::Transform3D Transform3D;

/**
 * SimulatedImageStreamer delivers a stream of 2D images
 * sliced from the incoming volume based on the tools positions.
 *
 * \date May 21, 2013
 * \author Janne Beate Bakeng, SINTEF
 */

class SimulatedImageStreamer : public ImageStreamer
{
	Q_OBJECT

public:
	SimulatedImageStreamer();
	virtual ~SimulatedImageStreamer();

	void initialize(ssc::ImagePtr image, ssc::ToolPtr tool);
	virtual bool startStreaming(SenderPtr sender);
	virtual void stopStreaming();

	virtual QString getType();

private slots:
	virtual void streamSlot();
	void sliceSlot(Transform3D matrix, double timestamp);

private:
	ssc::ImagePtr getSlice(ssc::ImagePtr source);

	ssc::ImagePtr mSourceImage;
	ssc::ToolPtr mTool;
	ssc::ImagePtr mImageToSend;

};
typedef boost::shared_ptr<SimulatedImageStreamer> SimulatedImageStreamerPtr;

} /* namespace cx */
#endif /* CXSIMULATEDIMAGESTREAMER_H_ */
