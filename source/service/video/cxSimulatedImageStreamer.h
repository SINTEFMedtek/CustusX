#ifndef CXSIMULATEDIMAGESTREAMER_H_
#define CXSIMULATEDIMAGESTREAMER_H_

#include "vtkSmartPointer.h"
#include "cxTransform3D.h"
#include "cxImageStreamer.h"
#include "cxCyclicActionLogger.h"

#include "cxConfig.h"
#ifdef CX_BUILD_US_SIMULATOR
#include "../../../../../UltrasoundSimulation/UltrasoundSimulation/ImageSimulator.h"
#endif //CX_BUILD_US_SIMULATOR

typedef vtkSmartPointer<class vtkImageMask> vtkImageMaskPtr;

namespace cx
{
typedef Transform3D Transform3D;
class DataManager;

/**
 * SimulatedImageStreamer delivers a stream of 2D images
 * sliced from the incoming volume based on the tools positions.
 *
 * \ingroup cx_service_video
 * \date May 21, 2013
 * \author Janne Beate Bakeng, SINTEF
 */

class SimulatedImageStreamer : public ImageStreamer
{
	Q_OBJECT

public:
	SimulatedImageStreamer();
	virtual ~SimulatedImageStreamer();

	void initialize(ImagePtr image, ToolPtr tool, DataServicePtr dataManager);
	virtual bool startStreaming(SenderPtr sender);
	virtual void stopStreaming();

	virtual QString getType();

	int getAverageTimePerSimulatedFrame();

private slots:
	virtual void streamSlot();
	void resetMask();
	void sliceSlot();
	void setSourceToImageSlot(QString imageUid);
	void defineSectorInSimulator();

private:
    void initUSSimulator();
	ImagePtr getSlice();
	vtkImageDataPtr getMask();

	vtkImageDataPtr frameGrab(ImagePtr source, bool applyLUT = true);
	Transform3D getTransform_vMr(); ///< from reference space to video image space
	void setSourceImage(ImagePtr image);
	ImagePtr calculateSlice(ImagePtr source);
	vtkImageDataPtr maskSlice(vtkImageDataPtr unmaskedSlice);
//	vtkImageDataPtr simulateUS(vtkImageDataPtr maskedFramedgrabbedSlice);
	vtkImageDataPtr simulateUSFromCTSlice(ImagePtr source);
	vtkImageDataPtr simulateUSFromMRSlice(ImagePtr source);
	void setSimulatorParameters();
	vtkImageDataPtr createSimulatorInputSlice(ImagePtr source);
	vtkImageDataPtr sliceOriginal(ImagePtr source);
	ImagePtr convertToSscImage(vtkImageDataPtr slice, ImagePtr volume);

	ImagePtr mSourceImage;
	ToolPtr mTool;
	mutable ImagePtr mCachedImageToSend;
	mutable vtkImageDataPtr mCachedMask;
	DataServicePtr mDataManager;

	CyclicActionLoggerPtr mTimer;///< Timer for timing parts of the simulation

#ifdef CX_BUILD_US_SIMULATOR
	boost::shared_ptr<ImageSimulator> mUSSimulator;
#endif //CX_BUILD_US_SIMULATOR

};
typedef boost::shared_ptr<SimulatedImageStreamer> SimulatedImageStreamerPtr;

} /* namespace cx */
#endif /* CXSIMULATEDIMAGESTREAMER_H_ */
