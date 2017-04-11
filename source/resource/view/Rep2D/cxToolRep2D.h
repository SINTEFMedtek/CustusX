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
		void setCrosshairColor(const QColor& color);

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
