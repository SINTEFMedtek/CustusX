/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/

#ifndef CXSTREAMREP3D_H
#define CXSTREAMREP3D_H

#include "cxVolumetricRep.h"
#include "cxForwardDeclarations.h"
#include "cxTransform3D.h"

namespace cx
{
typedef boost::shared_ptr<class VideoSourceGraphics> VideoSourceGraphicsPtr;

/** \brief Display a 3D stream in 3D
 *
 * \ingroup cx_resource_view
 * \ingroup cx_resource_view_rep3D
 *
 * \date jan 29, 2015
 * \author Ole Vegard Solberg, SINTEF
 */
class cxResourceVisualization_EXPORT StreamRep3D : public VolumetricRep
{
	Q_OBJECT
public:
	static StreamRep3DPtr New(SpaceProviderPtr spaceProvider, PatientModelServicePtr patientModelService, const QString &uid="");
	virtual QString getType() const;

	void setTrackedStream(TrackedStreamPtr trackedStream);
	TrackedStreamPtr getTrackedStream();

private slots:
	void newTool(ToolPtr tool);
	void newVideoSource(VideoSourcePtr videoSource);
private:
	StreamRep3D(SpaceProviderPtr spaceProvider, PatientModelServicePtr patientModelService);
	void initTransferFunction(ImagePtr image);
	void setVisualizerType();

	TrackedStreamPtr mTrackedStream;
	VideoSourcePtr mVideoSource;

	PatientModelServicePtr mPatientModelService;
};

} //cx

#endif // CXSTREAMREP3D_H
