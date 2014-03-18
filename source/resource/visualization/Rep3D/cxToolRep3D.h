// This file is part of SSC,
// a C++ Library supporting Image Guided Therapy Applications.
//
// Copyright (C) 2008- SINTEF Medical Technology
// Copyright (C) 2008- Sonowand AS
//
// SSC is owned by SINTEF Medical Technology and Sonowand AS,
// hereafter named the owners. Each particular piece of code
// is owned by the part that added it to the library.
// SSC source code and binaries can only be used by the owners
// and those with explicit permission from the owners.
// SSC shall not be distributed to anyone else.
//
// SSC is distributed WITHOUT ANY WARRANTY; without even
// the implied warranty of MERCHANTABILITY or FITNESS FOR
// A PARTICULAR PURPOSE.
//
// See sscLicense.txt for more information.

#ifndef CXTOOLREP3D_H_
#define CXTOOLREP3D_H_

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
 * \ingroup cx_resource_visualization
 * \ingroup cx_resource_visualization_rep3D
 */
class ToolRep3D : public RepImpl
{
	Q_OBJECT
public:
	static ToolRep3DPtr New(SpaceProviderPtr spaceProvider, const QString& uid, const QString& name="");
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
	ToolRep3D(SpaceProviderPtr spaceProvider, const QString& uid, const QString& name="");
	virtual void addRepActorsToViewRenderer(View *view);
	virtual void removeRepActorsFromViewRenderer(View *view);
	bool showProbe();
	virtual void onModifiedStartRender();

private slots:
//	/**
//	 * Receive transform and timestamp from a tool
//	 *\param matrix		The tools transform
//	 *\param timestamp	The time at which the transform was recorded
//	 */
//	void receiveTransforms(Transform3D matrix, double timestamp);
	void probeSectorChanged();
	/**
	 * Receive whether or not the tool is visible to the tracking system
	 *\param visible Whether or not the tool is visible to the tracking system.
	 */
	void receiveVisible(bool visible);
	void tooltipOffsetSlot(double val); ///< receive the virtual tool tip extension.

private:
	void update();
	void updateOffsetGraphics();
	void scaleSpheres();

	double mSphereRadius;
	ToolPtr mTool;
	vtkActorPtr mToolActor;
	vtkPolyDataMapperPtr mPolyDataMapper;
	vtkSTLReaderPtr mSTLReader;
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
	VideoSourceGraphicsPtr mRTStream;
	QColor mTooltipPointColor;
	QColor mOffsetPointColor;
	QColor mOffsetLineColor;
	int mStipplePattern;
};
} // namespace cx

#endif /*CXTOOLREP3D_H_*/
