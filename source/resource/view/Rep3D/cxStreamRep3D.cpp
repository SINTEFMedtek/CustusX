/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/


#include <vtkRenderer.h>
#include <vtkVolumeMapper.h>
#include <vtkImageData.h>

#include "cxStreamRep3D.h"
#include "cxTrackedStream.h"
#include "cxVideoSourceGraphics.h"
#include "cxView.h"
#include "cxTool.h"
#include "cxPatientModelService.h"
#include "cxVolumeProperty.h"
#include "cxReporter.h"
#include "cxSettings.h"

//To be removed
#include "cxImageTF3D.h"
#include "cxRegistrationTransform.h"


namespace cx
{

StreamRep3DPtr StreamRep3D::New(SpaceProviderPtr spaceProvider, PatientModelServicePtr patientModelService, const QString& uid)
{
	return wrap_new(new StreamRep3D(spaceProvider, patientModelService), uid);
}

StreamRep3D::StreamRep3D(SpaceProviderPtr spaceProvider, PatientModelServicePtr patientModelService) :
	VolumetricRep(),
	mTrackedStream(TrackedStreamPtr()),
	mPatientModelService(patientModelService)
{
	this->setVisualizerType();
}

void StreamRep3D::setTrackedStream(TrackedStreamPtr trackedStream)
{
	if(mTrackedStream)
	{
		disconnect(mTrackedStream.get(), &TrackedStream::newTool, this, &StreamRep3D::newTool);
		disconnect(mTrackedStream.get(), &TrackedStream::newVideoSource, this, &StreamRep3D::newVideoSource);
		disconnect(mTrackedStream.get(), &TrackedStream::newFrame, this, &StreamRep3D::vtkImageDataChangedSlot);
	}

	mTrackedStream = trackedStream;

	if(mTrackedStream)
	{
		connect(mTrackedStream.get(), &TrackedStream::newTool, this, &StreamRep3D::newTool);
		connect(mTrackedStream.get(), &TrackedStream::newVideoSource, this, &StreamRep3D::newVideoSource);
		connect(mTrackedStream.get(), &TrackedStream::newFrame, this, &StreamRep3D::vtkImageDataChangedSlot);
		this->newTool(mTrackedStream->getProbeTool());
		this->newVideoSource(mTrackedStream->getVideoSource());
	}
}

void StreamRep3D::newTool(ToolPtr tool)
{
}

void StreamRep3D::newVideoSource(VideoSourcePtr videoSource)
{
	if(!videoSource)
		return;

	ImagePtr image = mTrackedStream->getChangingImage();
	this->setImage(image);
//	mPatientModelService->insertData(image);

	this->initTransferFunction(image);

	mVideoSource = videoSource;
}

// Set transfer function so that we can see the volume change.
// Should be replaced with GUI for TrackedStream
void StreamRep3D::initTransferFunction(ImagePtr image)
{
	ImageTF3DPtr tf3D = image->getTransferFunctions3D();
	IntIntMap opacity;
	opacity[90] = 0;
	opacity[100] = 5;
	opacity[200] = image->getMaxAlphaValue();
	tf3D->resetAlpha(opacity);
	image->setTransferFunctions3D(tf3D);
}

TrackedStreamPtr StreamRep3D::getTrackedStream()
{
	return mTrackedStream;
}

QString StreamRep3D::getType() const
{
	return "StreamRep3D";
}

void StreamRep3D::setVisualizerType()
{
	QString visualizerType = settings()->value("View3D/ImageRender3DVisualizer").toString();
	if(visualizerType == "vtkVolumeTextureMapper3D")
		this->setUseVolumeTextureMapper();
	else if(visualizerType == "vtkGPUVolumeRayCastMapper")
		this->setUseGPUVolumeRayCastMapper();
	else
		reportError(QString("StreamRep3D::setVisualizerType(): No visualizer found for string=%1").arg(visualizerType));
}

} //cx
