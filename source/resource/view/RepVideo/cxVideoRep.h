/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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
