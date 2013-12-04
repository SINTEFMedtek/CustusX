#ifndef CXSIMULATEDIMAGESTREAMER_H_
#define CXSIMULATEDIMAGESTREAMER_H_

#include "vtkSmartPointer.h"
#include "sscTransform3D.h"
#include "cxImageStreamer.h"

typedef vtkSmartPointer<class vtkImageMask> vtkImageMaskPtr;

namespace cx
{
typedef Transform3D Transform3D;

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

	void initialize(); ///< initializes with active image and first probe
	void initialize(ImagePtr image, ToolPtr tool);
	virtual bool startStreaming(SenderPtr sender);
	virtual void stopStreaming();

	virtual QString getType();

private slots:
	virtual void streamSlot();
	void resetMask();
	void sliceSlot();
	void setSourceToActiveImageSlot();
	void setSourceToImageSlot(QString imageUid);

private:
	ImagePtr getSlice();
	vtkImageDataPtr getMask();

	vtkImageDataPtr frameGrab(ImagePtr source);
	Transform3D getTransform_vMr(); ///< from reference space to video image space
	void setSourceImage(ImagePtr image);
	ImagePtr calculateSlice(ImagePtr source);
	vtkImageDataPtr maskSlice(vtkImageDataPtr unmaskedSlice);
	ImagePtr convertToSscImage(vtkImageDataPtr slice, ImagePtr volume);

	ImagePtr mSourceImage;
	ToolPtr mTool;
	mutable ImagePtr mCachedImageToSend;
	mutable vtkImageDataPtr mCachedMask;

};
typedef boost::shared_ptr<SimulatedImageStreamer> SimulatedImageStreamerPtr;

} /* namespace cx */
#endif /* CXSIMULATEDIMAGESTREAMER_H_ */
