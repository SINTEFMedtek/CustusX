/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
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

	void setStayHiddenAfterVisible(bool val);
	void setStayVisibleAfterHide(bool val); ///< if true, tool is still rendered as visible after visibility status is hidden.
	void setOffsetPointVisibleAtZeroOffset(bool val); ///< if true the sphere is visible even when the offset is zero
	void setSphereRadiusInNormalizedViewport(bool on);
	void setTooltipPointColor(const QColor& color);
	void setToolOffsetPointColor(const QColor& color);
	void setToolOffsetLineColor(const QColor& color);

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
