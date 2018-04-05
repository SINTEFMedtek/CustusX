/*=========================================================================
This file is part of CustusX, an Image Guided Therapy Application.
                 
Copyright (c) SINTEF Department of Medical Technology.
All rights reserved.
                 
CustusX is released under a BSD 3-Clause license.
                 
See Lisence.txt (https://github.com/SINTEFMedtek/CustusX/blob/master/License.txt) for details.
=========================================================================*/



#ifndef CXTOOLREP2D_H_
#define CXTOOLREP2D_H_

#include "cxResourceVisualizationExport.h"

#include "vtkForwardDeclarations.h"
#include "cxForwardDeclarations.h"
#include "cxVtkHelperClasses.h"
#include "cxRepImpl.h"
#include "cxTransform3D.h"
#include "cxBoundingBox3D.h"

namespace cx
{
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
 * \ingroup cx_resource_view
 * \ingroup cx_resource_view_rep2D
 */
class cxResourceVisualization_EXPORT ToolRep2D : public RepImpl
{
	Q_OBJECT
	public:
		static ToolRep2DPtr New(SpaceProviderPtr spaceProvider, const QString& uid="");
		~ToolRep2D();
		virtual QString getType() const;

		void setSliceProxy(SliceProxyPtr slicer);
		void setViewportData(const Transform3D& vpMs, const DoubleBoundingBox3D& vp);

		void setUseCrosshair(bool on);
		void setCrosshairColor(const QColor& color);
		void setTooltipLineColor(const QColor& color);
		void setTooltipPointColor(const QColor& color);
		void setToolOffsetPointColor(const QColor& color);
		void setToolOffsetLineColor(const QColor& color);

	private slots:
		void sliceTransformChangedSlot(Transform3D sMr); 
		void toolTransformAndTimestampSlot(Transform3D prMt, double timestamp); 
		void toolVisibleSlot(bool visible); 
		void update();

	protected:
		bool showProbe();
		bool showOffset();

		virtual void addRepActorsToViewRenderer(ViewPtr view);
		virtual void removeRepActorsFromViewRenderer(ViewPtr view);
		virtual void onModifiedStartRender();

	private:
		ToolRep2D(SpaceProviderPtr spaceProvider);

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
