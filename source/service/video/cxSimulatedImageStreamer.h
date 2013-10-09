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
	void generateMaskSlot();
	void sliceSlot();
	void setSourceToActiveImageSlot();
	void setSourceToImageSlot(QString imageUid);

private:
	void setSourceImage(ImagePtr image);
	ImagePtr getSlice(ImagePtr source);
	vtkMatrix4x4Ptr calculateSliceAxes();
	vtkImageDataPtr getSliceUsingProbeDefinition(ImagePtr source, vtkMatrix4x4Ptr sliceAxes);
	vtkImageDataPtr maskSlice(vtkImageDataPtr unmaskedSlice);
	ImagePtr convertToSscImage(vtkImageDataPtr slice, ImagePtr volume);
	vtkImageReslicePtr createReslicer(ImagePtr source, vtkMatrix4x4Ptr sliceAxes);
	Transform3D getTransformFromProbeSectorImageSpaceToImageSpace();

	ImagePtr mSourceImage;
	ToolPtr mTool;
	ImagePtr mImageToSend;
	vtkImageDataPtr mMask;

};
typedef boost::shared_ptr<SimulatedImageStreamer> SimulatedImageStreamerPtr;

} /* namespace cx */
#endif /* CXSIMULATEDIMAGESTREAMER_H_ */
