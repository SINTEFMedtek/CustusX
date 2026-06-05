/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.

CustusX is released under a BSD 3-Clause license.

See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXVOLUMESLICESTREAMER_H
#define CXVOLUMESLICESTREAMER_H

#include "vtkSmartPointer.h"
#include "cxTransform3D.h"
#include "cxStreamer.h"
#include "cxCyclicActionLogger.h"
#include "cxForwardDeclarations.h"
#include "cxGrabberExport.h"

typedef vtkSmartPointer<class vtkImageMask> vtkImageMaskPtr;

namespace cx
{

/**
 * VolumeSliceStreamer delivers a stream of 2D images sliced from a volume
 * based on the probe tool position. No physics simulation is applied;
 * the slice is the raw resampled data masked to the probe sector shape.
 */
class cxGrabber_EXPORT VolumeSliceStreamer : public Streamer
{
	Q_OBJECT

public:
	VolumeSliceStreamer(PatientModelServicePtr patientModelService);
	virtual ~VolumeSliceStreamer();

	void setSourceImage(ImagePtr image);
	bool initialize(ImagePtr image, ToolPtr tool);
	virtual void startStreaming(SenderPtr sender);
	virtual void stopStreaming();
	virtual bool isStreaming();

private slots:
	virtual void streamSlot();
	void resetMask();
	void sliceSlot();

private:
	bool initialize(ToolPtr tool);
	ImagePtr getSlice();
	vtkImageDataPtr getMask();
	vtkImageDataPtr frameGrab(ImagePtr source);
	Transform3D getTransform_vMr();
	ImagePtr calculateSlice(ImagePtr source);
	vtkImageDataPtr maskSlice(vtkImageDataPtr unmaskedSlice);
	vtkImageDataPtr sliceOriginal(ImagePtr source);
	ImagePtr convertToCxImage(vtkImageDataPtr slice, ImagePtr volume);

	ImagePtr mSourceImage;
	ToolPtr mTool;
	mutable ImagePtr mCachedImageToSend;
	mutable vtkImageDataPtr mCachedMask;
	PatientModelServicePtr mPatientModelService;

	CyclicActionLoggerPtr mTimer;
};
typedef boost::shared_ptr<VolumeSliceStreamer> VolumeSliceStreamerPtr;

} /* namespace cx */
#endif /* CXVOLUMESLICESTREAMER_H */
