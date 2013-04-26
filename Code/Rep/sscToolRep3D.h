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

#ifndef SSCTOOL3DREP_H_
#define SSCTOOL3DREP_H_

#include "sscRepImpl.h"
#include "sscTransform3D.h"

#include "vtkForwardDeclarations.h"
#include "sscForwardDeclarations.h"
#include "sscVtkHelperClasses.h"
#include "sscViewportListener.h"

namespace ssc
{

typedef boost::shared_ptr<class VideoSourceGraphics> VideoSourceGraphicsPtr;
typedef boost::shared_ptr<class ToolTracer> ToolTracerPtr;

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
 * \ingroup sscRep
 * \ingroup sscRep3D
 */
class ToolRep3D : public RepImpl
{
	Q_OBJECT
public:
	static ToolRep3DPtr New(const QString& uid, const QString& name="");
	virtual ~ToolRep3D();
	virtual QString getType() const;

	virtual void setTool(ToolPtr tool);
	virtual bool hasTool(ToolPtr tool) const;
	virtual ToolPtr getTool();
	ToolTracerPtr getTracer();
	void setSphereRadius(double radius);

	void setTooltipPointColor(Vector3D c) { mTooltipPointColor = c; }
	void setOffsetPointColor(Vector3D c) { mOffsetPointColor = c; }
	void setOffsetLineColor(Vector3D c) { mOffsetLineColor = c; }
	void setOffsetStipplePattern(int pattern) { mStipplePattern = pattern; }

	void setStayHiddenAfterVisible(bool val);
	void setStayVisibleAfterHide(bool val); ///< if true, tool is still rendered as visible after visibility status is hidden.
	void setOffsetPointVisibleAtZeroOffset(bool val); ///< if true the sphere is visible even when the offset is zero
	void setSphereRadiusInNormalizedViewport(bool on);

protected:
	ToolRep3D(const QString& uid, const QString& name="");
	virtual void addRepActorsToViewRenderer(View *view);
	virtual void removeRepActorsFromViewRenderer(View *view);
	bool showProbe();

private slots:
	/**
	 * Receive transform and timestamp from a tool
	 *\param matrix		The tools transform
	 *\param timestamp	The time at which the transform was recorded
	 */
	void receiveTransforms(Transform3D matrix, double timestamp);
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
	ssc::ViewportListenerPtr mViewportListener;

	//US Probe sector
	ProbeSectorPtr mProbeSector;
	vtkPolyDataMapperPtr mProbeSectorPolyDataMapper;
	vtkActorPtr mProbeSectorActor;
	VideoSourceGraphicsPtr mRTStream;
	Vector3D mTooltipPointColor;
	Vector3D mOffsetPointColor;
	Vector3D mOffsetLineColor;
	int mStipplePattern;
};
} // namespace ssc

#endif /*SSCTOOL3DREP_H_*/
