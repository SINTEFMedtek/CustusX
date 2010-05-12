//
// C++ Interface: vmTool2drep
//
// Description:
//
//
// Author:  <>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef VMVMTOOL2DREP_H
#define VMVMTOOL2DREP_H


#include <vtkSmartPointer.h>
typedef vtkSmartPointer<class vtkRenderer> vtkRendererPtr;

#include "sscRepImpl.h"
#include "sscTransform3D.h"
#include "sscBoundingBox3D.h"
#include "sscProbeSector.h"

namespace ssc
{
// forward declarations
typedef boost::shared_ptr<class CrossHair2D> CrossHair2DPtr;
typedef boost::shared_ptr<class LineSegment> LineSegmentPtr;
typedef boost::shared_ptr<class TextDisplay> TextDisplayPtr;
typedef boost::shared_ptr<class SliceProxy> SliceProxyPtr;
typedef boost::shared_ptr<class OffsetPoint> OffsetPointPtr;

typedef boost::shared_ptr<class ToolRep2D> ToolRep2DPtr;
typedef boost::shared_ptr<class USProbe2D> USProbe2DPtr;

/**
 *This class will hold all the representation and drawing of 2d tool
 *in navigation and registration.
 *
 * Used by Sonowand.
 */
class ToolRep2D : public ssc::RepImpl
{
	Q_OBJECT
	public:
		static ToolRep2DPtr New(const std::string& uid, const std::string& name="");
		~ToolRep2D();
		virtual std::string getType() const;

		void setSliceProxy(ssc::SliceProxyPtr slicer);
		void setViewportData(const Transform3D& vpMs, const DoubleBoundingBox3D& vp);

		void setUseOffset(bool on);
		void setUseCrosshair(bool on);
		void setUseToolLine(bool on);
		void setUseOffsetText(bool on);

	private slots:
		void sliceTransformChangedSlot(Transform3D sMr); 
		void toolTransformAndTimestampSlot(Transform3D prMt, double timestamp); 
		void toolVisibleSlot(bool visible); 
		void update();
	protected:
		bool showProbe();
		bool showOffset();

		virtual void addRepActorsToViewRenderer(ssc::View* view);
		virtual void removeRepActorsFromViewRenderer(ssc::View* view);		
	private:
		void setProbeSector(ssc::ProbeSector data);				
		double getOffset();
		void createCrossHair(vtkRendererPtr renderer);
		void setVisibility();
		void createToolLine(vtkRendererPtr renderer, const Vector3D& centerPos );		
		void createOffsetText(vtkRendererPtr renderer, const Vector3D& pos );
		void createUSProbe(vtkRendererPtr renderer);
		
		void updateToolLine(const Vector3D& crossPos, const Vector3D& toolTipPos, const Vector3D toolTipBackPos);
		void updateOffsetText();
		void updateUSProbe(const Transform3D& vpMt);		
		ToolRep2D(const std::string& uid, const std::string& name="");
		void crossHairResized();

		ssc::SliceProxyPtr mSlicer;
		Transform3D m_vpMs;	
		DoubleBoundingBox3D mBB_vp;
		
		bool mUseOffset;
		bool mUseCrosshair; 
		bool mUseToolLine; 
		bool mUseOffsetText;		
//		bool mUseProbe;
		
		//double mOffset; 
		CrossHair2DPtr cursor;
		LineSegmentPtr center2Tool;
		LineSegmentPtr tool2Back;
		OffsetPointPtr centerPoint;
		OffsetPointPtr toolPoint;
		TextDisplayPtr distanceText;
		USProbe2DPtr mUSProbe2D;
};


} // namespace vm

#endif
