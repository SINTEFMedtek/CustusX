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


#ifndef CXTOOLREP2D_H_
#define CXTOOLREP2D_H_

#include "vtkForwardDeclarations.h"
#include "cxForwardDeclarations.h"
#include "cxVtkHelperClasses.h"
#include "cxRepImpl.h"
#include "cxTransform3D.h"
#include "cxBoundingBox3D.h"

namespace cx
{
class ProbeData;
typedef boost::shared_ptr<class SpaceProvider> SpaceProviderPtr;


/**\brief Display a Tool in 2D.
 *
 * Toolrep2D displays a Tool as a line, with points showing
 * the toop tip and the offset point. If the Tool is a Probe,
 * the probe sector can be shown as well.
 *
 * Several optional visualizations are available.
 *
 * \image html sscArchitecture_tool.png "ToolRep2D, ToolRep3D and surrounding classes."
 *
 * Used by CustusX and Sonowand.
 *
 * \ingroup cx_resource_visualization
 * \ingroup cx_resource_visualization_rep2D
 */
class ToolRep2D : public RepImpl
{
	Q_OBJECT
	public:
		static ToolRep2DPtr New(SpaceProviderPtr spaceProvider, const QString& uid, const QString& name="");
		~ToolRep2D();
		virtual QString getType() const;

		void setSliceProxy(SliceProxyPtr slicer);
		void setViewportData(const Transform3D& vpMs, const DoubleBoundingBox3D& vp);

		void setUseOffset(bool on);
		void setUseCrosshair(bool on);
		void setUseToolLine(bool on);
		void setUseOffsetText(bool on);
		void setMergeOffsetAndToolLine(bool on);
		void setTooltipPointColor(QColor c) { mTooltipPointColor = c; }
		void setOffsetPointColor(QColor c) { mOffsetPointColor = c; }
		void setTooltipLineColor(QColor c) { mTooltipLineColor = c; }
		void setOffsetLineColor(QColor c) { mOffsetLineColor = c; }
		void setOffsetStipplePattern(int pattern) { mStipplePattern = pattern; }

	private slots:
		void sliceTransformChangedSlot(Transform3D sMr); 
		void toolTransformAndTimestampSlot(Transform3D prMt, double timestamp); 
		void toolVisibleSlot(bool visible); 
		void update();

	protected:
		bool showProbe();
		bool showOffset();

		virtual void addRepActorsToViewRenderer(View *view);
		virtual void removeRepActorsFromViewRenderer(View *view);
		virtual void onModifiedStartRender();

	private:
		ToolRep2D(SpaceProviderPtr spaceProvider, const QString& uid, const QString& name="");

		void setProbeSector(ProbeData data);
		double getOffset();
		void createCrossHair(vtkRendererPtr renderer);
		void setVisibility();
		void createToolLine(vtkRendererPtr renderer, const Vector3D& centerPos );
		void createOffsetText(vtkRendererPtr renderer, const Vector3D& pos );

		void updateToolLine(const Vector3D& crossPos, const Vector3D& toolTipPos, const Vector3D toolTipBackPos);
		void updateOffsetText();
		void crossHairResized();

		SpaceProviderPtr mSpaceProvider;
		SliceProxyPtr mSlicer;
		Transform3D m_vpMs;
		DoubleBoundingBox3D mBB_vp;

		bool mUseOffset;
		bool mUseCrosshair; 
		bool mUseToolLine; 
		bool mUseOffsetText;
		bool mMergeOffsetAndToolLine;

		RGBColor mTooltipPointColor;
		RGBColor mOffsetPointColor;
		RGBColor mTooltipLineColor;
		RGBColor mOffsetLineColor;
		int mStipplePattern;

		CrossHair2DPtr cursor;
		LineSegmentPtr center2Tool;
		LineSegmentPtr tool2Back;
		OffsetPointPtr centerPoint;
		OffsetPointPtr toolPoint;
		TextDisplayPtr distanceText;

		//US Probe sector
		ProbeSectorPtr mProbeSector;
		vtkPolyDataMapperPtr mProbeSectorPolyDataMapper;
		vtkActorPtr mProbeSectorActor;
};


} // namespace vm

#endif
