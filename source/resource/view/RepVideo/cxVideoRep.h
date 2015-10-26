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


#ifndef CXVIDEOREP_H_
#define CXVIDEOREP_H_

#include "cxResourceVisualizationExport.h"

#include "cxRepImpl.h"
#include "cxVtkHelperClasses.h"
#include "cxForwardDeclarations.h"
#include "cxProbeSector.h"

namespace cx
{
typedef boost::shared_ptr<class ViewportListener> ViewportListenerPtr;
typedef boost::shared_ptr<class VideoSourceGraphics> VideoSourceGraphicsPtr;

typedef boost::shared_ptr<class VideoFixedPlaneRep> VideoFixedPlaneRepPtr;


/** \brief Display a VideoSource in a View.
 *
 * A rep visualizing a VideoSource directly into the view plane.
 * It does not follow the tool, but controls the camera in order to
 * fill the entire View.
 *
 * Used by CustusX.
 *
 * \ingroup cx_resource_view
 * \ingroup cx_resource_view_repvideo
 *
 * \date Oct 31, 2010
 * \author christiana
 */
class cxResourceVisualization_EXPORT VideoFixedPlaneRep : public RepImpl
{
	Q_OBJECT
public:
	VideoFixedPlaneRep(const QString& uid, const QString& name="");
	virtual ~VideoFixedPlaneRep();
	virtual QString getType() const { return "RealTimeStreamFixedPlaneRep"; }
	void setRealtimeStream(VideoSourcePtr data);
	void setTool(ToolPtr tool);

	void setShowSector(bool on);
	bool getShowSector() const;

protected:
	virtual void addRepActorsToViewRenderer(ViewPtr view);
	virtual void removeRepActorsFromViewRenderer(ViewPtr view);
	private slots:
	void newDataSlot();

private:
	void setCamera();
	void updateSector();

	VideoSourceGraphicsPtr mRTGraphics;
	bool mShowSector;

	ToolPtr mTool;
	ProbeSector mProbeDefinition;
	VideoSourcePtr mData;

	TextDisplayPtr mStatusText;
	TextDisplayPtr mInfoText;
	TextDisplayPtr mOrientationVText;

	GraphicalPolyData3DPtr mProbeSector;
	GraphicalPolyData3DPtr mProbeOrigin;
	GraphicalPolyData3DPtr mProbeClipRect;

	vtkRendererPtr mRenderer;
	ViewportListenerPtr mViewportListener;
};


} // namespace cx

#endif /* CXVIDEOREP_H_ */
