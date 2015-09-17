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


#ifndef CXTOOLREP3D_H_
#define CXTOOLREP3D_H_

#include "cxResourceVisualizationExport.h"

#include "cxRepImpl.h"
#include "cxTransform3D.h"

#include "vtkForwardDeclarations.h"
#include "cxForwardDeclarations.h"
#include "cxVtkHelperClasses.h"
#include "cxViewportListener.h"

namespace cx
{

typedef boost::shared_ptr<class VideoSourceGraphics> VideoSourceGraphicsPtr;
typedef boost::shared_ptr<class ToolTracer> ToolTracerPtr;
typedef boost::shared_ptr<class SpaceProvider> SpaceProviderPtr;


/**\brief Display a Tool in 3D.
 *
 * ToolRep3D displays a Tool using the supplied graphical
 * representation from the Tool interface, with points showing
 * the tooltip and offset point. If the Tool is a Probe,
 * the probe sector and the prope video can be shown as well.
 *
 * Several optional visualizations are available.
 *
 * \image html sscArchitecture_tool.png "ToolRep2D, ToolRep3D and surrounding classes."
 *
 *
 * Used by CustusX and Sonowand.
 *
 * \ingroup cx_resource_view
 * \ingroup cx_resource_view_rep3D
 */
class cxResourceVisualization_EXPORT ToolRep3D : public RepImpl
{
	Q_OBJECT
public:
	static ToolRep3DPtr New(SpaceProviderPtr spaceProvider, const QString& uid="");
	virtual ~ToolRep3D();
	virtual QString getType() const;

	virtual void setTool(ToolPtr tool);
	virtual bool hasTool(ToolPtr tool) const;
	virtual ToolPtr getTool();
	ToolTracerPtr getTracer();
	void setSphereRadius(double radius);

	void setTooltipPointColor(QColor c);
	void setOffsetPointColor(QColor c);
	void setOffsetLineColor(QColor c);
	void setOffsetStipplePattern(int pattern);

	void setStayHiddenAfterVisible(bool val);
	void setStayVisibleAfterHide(bool val); ///< if true, tool is still rendered as visible after visibility status is hidden.
	void setOffsetPointVisibleAtZeroOffset(bool val); ///< if true the sphere is visible even when the offset is zero
	void setSphereRadiusInNormalizedViewport(bool on);

protected:
	ToolRep3D(SpaceProviderPtr spaceProvider);
	virtual void addRepActorsToViewRenderer(ViewPtr view);
	virtual void removeRepActorsFromViewRenderer(ViewPtr view);
	bool showProbe();
	virtual void onModifiedStartRender();

private slots:
	void probeSectorChanged();
	/**
	 * Receive whether or not the tool is visible to the tracking system
	 *\param visible Whether or not the tool is visible to the tracking system.
	 */
	void receiveVisible(bool visible);
	void tooltipOffsetSlot(double val); ///< receive the virtual tool tip extension.
	void updateToolActor();

private:
	void update();
	void updateOffsetGraphics();
	void scaleSpheres();

	double mSphereRadius;
	ToolPtr mTool;
	vtkActorPtr mToolActor;
	vtkPolyDataMapperPtr mPolyDataMapper;
	GraphicalPoint3DPtr mTooltipPoint;
	GraphicalPoint3DPtr mOffsetPoint;
	GraphicalLine3DPtr mOffsetLine;
	bool mStayHiddenAfterVisible;
	bool mStayVisibleAfterHide;
	bool mOffsetPointVisibleAtZeroOffset;
	bool mSphereRadiusInNormalizedViewport;
	ToolTracerPtr mTracer;
	ViewportListenerPtr mViewportListener;
	SpaceProviderPtr mSpaceProvider;

	//US Probe sector
	ProbeSectorPtr mProbeSector;
	vtkPolyDataMapperPtr mProbeSectorPolyDataMapper;
	vtkActorPtr mProbeSectorActor;
	QColor mTooltipPointColor;
	QColor mOffsetPointColor;
	QColor mOffsetLineColor;
	int mStipplePattern;
};
} // namespace cx

#endif /*CXTOOLREP3D_H_*/
