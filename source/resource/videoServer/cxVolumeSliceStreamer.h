/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.

Copyright (c) 2008-2014, SINTEF Department of Medical Technology
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors
   may be used to endorse or promote products derived from this software
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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

	int getAverageTimePerSimulatedFrame();

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
	ImagePtr convertToSscImage(vtkImageDataPtr slice, ImagePtr volume);

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
